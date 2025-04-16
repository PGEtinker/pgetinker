import { WebSocketServer, ServerOptions } from 'ws';
import { IncomingMessage, Server } from 'node:http';
import express from 'express';
import { URL } from 'node:url';
import { Socket } from 'node:net';
import { IWebSocket, WebSocketMessageReader, WebSocketMessageWriter } from 'vscode-ws-jsonrpc';
import { createConnection, createServerProcess, forward } from 'vscode-ws-jsonrpc/server';
import { Message, InitializeRequest, InitializeParams, DiagnosticRelatedInformation, Diagnostic, PublishDiagnosticsNotification, PublishDiagnosticsParams } from 'vscode-languageserver';
import * as cp from 'child_process';
import { readFileSync, writeFileSync, existsSync, mkdirSync } from 'node:fs';
import path from 'node:path';
import cookie from "cookie";
import * as crypto from "node:crypto";

import "dotenv/config";

enum RunMode {
    development = "development",
    production = "production"
}

const mode: RunMode | string = process.env.MODE ||  RunMode.production;

function isProduction() : boolean
{
    return mode == RunMode.production;
}

function isDevelopment() : boolean
{
    return !isProduction();
}

interface LanguageServerRunConfig {
    serverName: string;
    pathName: string;
    serverPort: number;
    wsServerOptions: ServerOptions,
    spawnOptions?: cp.SpawnOptions;
}

function log(...args: any[])
{
    if(isDevelopment())
    {
        console.log(...args);
    }
}

function filterLink(link: string)
{
    [
        "/opt/emsdk/upstream/emscripten/cache/sysroot",
    ].forEach((value) =>
    {
        link = link.replace(value, "/***");
    });

    return link;
}

/**
 * start the language server inside the current process
 */
const launchLanguageServer = (runconfig: LanguageServerRunConfig, socket: IWebSocket, libraries: any) => {
    
    if(!libraries)
    {
        console.log("no libraries defined");
        return;
    }
        

    /**
     * Process the libraries and break it out into their version directories
     */
    let baseLibraryDirectory = process.env.PGETINKER_LIBS_DIRECTORY || "/opt/libs";

    baseLibraryDirectory += "/olcPixelGameEngine/" + libraries["olcPixelGameEngine"];
    
    const libraryDirectories: any = {};
    libraryDirectories["olcPixelGameEngine"] = baseLibraryDirectory + "/olcPixelGameEngine";

    const libraryKeys = Object.keys(libraries);

    libraryKeys.forEach((library) =>
    {
        if(library == "olcPixelGameEngine")
            return;
        
        libraryDirectories[library] = baseLibraryDirectory + "/" + library + "/" + libraries[library];
    });

    // create a sha256 of the libraryDirectories object
    const libraryHash = crypto.createHash("sha256");
    
    libraryHash.update(JSON.stringify(libraryDirectories));
    
    // use the hash to derive a workspace
    const workspacePath = path.join(process.cwd(), "workspaces", libraryHash.digest("base64url"));

    const { serverName, spawnOptions } = runconfig;
    const errors: string[] = [];

    let nsJailArgs = [
        "--config",
        path.join(process.cwd(), process.env.COMPILER_NSJAIL_CFG || "nsjail-emscripten.cfg"),
        "-B",
        `${workspacePath}:/workspace`,
    ];
    
    libraryKeys.forEach((library) =>
    {
        if(!existsSync(libraryDirectories[library]))
        {
            errors.push(`${libraryDirectories[library]} does not exist.`);
            return;
        }

        nsJailArgs.push("-R");
        nsJailArgs.push(`${libraryDirectories[library]}:/workspace/${library}`);
    });
    
    if(isProduction())
    {
        // run nsjail in "really quiet" mode during production.
        nsJailArgs.push("-Q");    
    }
    
    nsJailArgs.push("--");
    
    // begin clangd specifics
    nsJailArgs.push("/usr/bin/clangd");
    nsJailArgs.push("--compile-commands-dir=/workspace");
    nsJailArgs.push("--header-insertion=never");

    // if we make it here, and have errors, quit
    if(errors.length > 0)
    {
        console.error(errors);
        return;
    }
    
    // if workspace doesn't exist, let's create it
    if(!existsSync(path.join(workspacePath, "compile_commands.json")))
    {
        if(!existsSync(path.join(process.cwd(), "workspaces")))
            mkdirSync(path.join(process.cwd(), "workspaces"));

        if(!existsSync(workspacePath))
            mkdirSync(workspacePath);
        
        let compileCommandsTemplate: string = readFileSync(path.join(process.cwd(), "compile_commands.template"), "utf-8");
        writeFileSync(path.join(workspacePath, "compile_commands.json"), compileCommandsTemplate);
    }

    log(libraryDirectories);
    log(libraries);
    log(workspacePath, existsSync(workspacePath));
    log(nsJailArgs);

    const reader = new WebSocketMessageReader(socket);
    const writer = new WebSocketMessageWriter(socket);
    
    // start the language server as an external process
    const socketConnection = createConnection(reader, writer, () => socket.dispose());
    const serverConnection = createServerProcess(serverName, "nsjail", nsJailArgs, spawnOptions);

    if (serverConnection)
    {
        forward(socketConnection, serverConnection, (message: Message) =>
        {
            if (Message.isRequest(message))
            {
                log(`${serverName} Server received:`);
                log(message);
                
                if(message.method === InitializeRequest.type.method)
                {
                    const initializeParams = message.params as InitializeParams;
                    initializeParams.processId = process.pid;
                }
            }
            
            if(Message.isNotification(message))
            {
                log(`${serverName} Sending Notification:`);

                if(message.method === PublishDiagnosticsNotification.method)
                {
                    const publishParams = message.params as PublishDiagnosticsParams;
                    
                    log("-- BEGIN DIAGNOSTICS --");
                    if(publishParams.diagnostics.length > 0)
                    {
                        publishParams.uri = filterLink(publishParams.uri);
                        log(publishParams.uri);

                        publishParams.diagnostics.forEach((diagnostic: Diagnostic) =>
                        {
                            log(diagnostic);

                            if(diagnostic?.relatedInformation && diagnostic.relatedInformation.length > 0)
                            {
                                diagnostic.relatedInformation.forEach((relatedInformation: DiagnosticRelatedInformation) =>
                                {
                                    relatedInformation.location.uri = filterLink(relatedInformation.location.uri)
                                })
                                
                            }
                        });
                    }
                    log("-- END DIAGNOSTICS --");
                }
            }

            if(Message.isResponse(message))
            {
                if(message.result)
                {
                    log(`${serverName} Server sent:`);

                    if((message.result as []).length > 0)
                    {
                        (message.result as []).forEach((item) =>
                        {
                            let keys = Object.keys(item);
    
                            if(keys.includes("target"))
                            {
                                (item as any).target = `unavailable`;
                            }
                            return undefined;
                        });
                    }

                    if((message.result as any).contents?.value)
                    {
                        // @ts-ignore
                        message.result.contents.value = filterLink(message.result.contents.value);
                    }
                    
                    log(message);
                }
                
            }
            
            return message;
        });
    }
};

const upgradeWsServer = (runconfig: LanguageServerRunConfig,
    config: {
        server: Server,
        wss: WebSocketServer
    }) =>
{
    config.server.on('upgrade', (request: IncomingMessage, socket: Socket, head: Buffer) =>
    {
        const baseURL = `http://${request.headers.host}/`;
        const pathName = request.url ? new URL(request.url, baseURL).pathname : undefined;

        if(pathName !== runconfig.pathName)
            return;

        config.wss.handleUpgrade(request, socket, head, webSocket =>
        {
            let libraries = null;

            try
            {
                let cookies = cookie.parse(request.headers["cookie"] as string);
                libraries = JSON.parse(decodeURIComponent(cookies.pgetinker_libraries));
            }
            catch(e)
            {

            }
            
            let keepAliveInterval: NodeJS.Timeout;
            
            const socket: IWebSocket = {
                send: content => webSocket.send(content, error => {
                    if (error) {
                        throw error;
                    }
                }),
                onMessage: cb => webSocket.on('message', (data) => {
                    log(data.toString());
                    cb(data);
                }),
                onError: cb => webSocket.on('error', cb),
                onClose: cb => webSocket.on('close', cb),
                dispose: () =>
                {
                    clearInterval(keepAliveInterval);
                    webSocket.close();
                }
            };

            // launch the server when the web socket is opened
            if (webSocket.readyState === webSocket.OPEN)
            {
                launchLanguageServer(runconfig, socket, libraries);
            }
            else
            {
                webSocket.on('open', () =>
                {
                    launchLanguageServer(runconfig, socket, libraries);
                });
            }

            keepAliveInterval = setInterval(() =>
            {
                webSocket.send(JSON.stringify({
                    jsonrpc: "2.0",
                    method: "telemetry/event", 
                    params: {
                        message: "Number Five Alive",
                    },
                }));
            }, 30000);
        });
    });
};

/** LSP server runner */
const runLanguageServer = (
    languageServerRunConfig: LanguageServerRunConfig
) => {
    process.on('uncaughtException', (error) =>
    {
        console.error('Uncaught Exception: ', error.toString());
        if (error.stack) {
            console.error(error.stack);
        }
    });

    // create the express application
    const app = express();

    // start the http server
    const httpServer: Server = app.listen(languageServerRunConfig.serverPort);
    const wss = new WebSocketServer(languageServerRunConfig.wsServerOptions);
    
    // create the web socket
    upgradeWsServer(languageServerRunConfig, {
        server: httpServer,
        wss
    });

    app.get("/trigger-close-clients", (_, response) =>
    {
        wss.clients.forEach((ws) =>
        {
            if(ws.OPEN)
            {
                ws.close();
            }
        });
        
        response.json({ message: "clients have been closed." });
    });

    process.on("SIGINT", () =>
    {
        wss.clients.forEach((ws) =>
        {
            ws.close();
        });
        wss.close();

        process.exit();
    });
    
};

runLanguageServer({
    serverName: 'CLANGD',
    pathName: '/clangd',
    serverPort: 3000,
    wsServerOptions: {
        noServer: true,
        perMessageDeflate: false,
        clientTracking: true,
        verifyClient: (clientInfo: { origin: string; secure: boolean; req: IncomingMessage }, callback) => 
        {
            try
            {
                let cookies = cookie.parse(clientInfo.req.headers["cookie"] as string);
                
                log("APP_KEY:", process.env.APP_KEY);

                let appKey  = Buffer.from(process.env.APP_KEY?.replace("base64:", "") as string, 'base64');
                
                let session = JSON.parse(Buffer.from(cookies.pgetinker_session, "base64").toString());
                
                log("SESSION:", session);
                
                // @ts-ignore
                const decipher = crypto.createDecipheriv(
                    "aes-256-cbc",
                    appKey,
                    Buffer.from(session.iv, 'base64')
                );
                    
                let plaintext = decipher.update(session.value, "base64", "utf8");
                plaintext += decipher.final("utf8");
            }
            catch(e)
            {
                // callback(false);
                return;
            }

            callback(true);
            return;
        }
    }
});
