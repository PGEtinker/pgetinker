import examples from "../lib/exampleCodes";
import { getUserConfiguration } from "../lib/monacoConfig";
import { configureMonacoWorkers, runCppWrapper } from "../lib/monacoWrapper";
import { getStorageValue, setStorageValue } from "../lib/storage";
import * as vscode from "vscode";
import { createToast, ToastType } from '../lib/createToast';
import axios from "axios";
import Cookies from 'js-cookie';

function extractSlugFromURI(uri)
{
    // Check if /s/ exists in the URI
    if (uri.includes('/s/'))
    {
        const parts = uri.split('/s/');
        const slug = parts[1]?.split('/')[0];
        return slug || null;
    }
    return null; // Return null if /s/ is not found
}

export default class EditorPanel
{
    state;
    autoConnect = true;
    monacoWrapper = null;
    maxFileSize = 50000;
    reconnectInterval = null;
    sharedSlug = null;

    constructor(state)
    {
        this.state = state;
        
        // reset editor font zoom
        window.addEventListener("keydown", (event) => {
            if (event.ctrlKey && event.key == "0") {
                vscode.commands.executeCommand("editor.action.fontZoomReset");
            }
        });
        
        configureMonacoWorkers();
        
        this.sharedSlug = extractSlugFromURI(window.location.href);
    }
    
    getValue()
    {
        return this.monacoWrapper.getEditor().getValue();
    }
    
    setValue(value)
    {
        this.monacoWrapper.getEditor().setValue(value);
    }
    
    async setToExample(codeId, codeName)
    {
        const codeIds = Object.keys(examples);
        for(let i = 0; i < codeIds.length; i++)
        {
            if(codeIds[i] === codeId)
            {
                await this.stopLanguageClient();
                this.state.editorPanel.setValue(examples[codeId]);
                this.state.editorPanel.reveal({ column: 1, lineNumber: 1 });
                
                if(this.state.playerPanel.running)
                {
                    document.querySelector("#start-stop").dispatchEvent(new Event("click"));                    
                }
                
                createToast(`Set Code to ${codeName}`, ToastType.Info);
                await this.startLanguageClient();
                return;
            }
        }
    }

    async stopLanguageClient()
    {
        const lcWrapper = this.monacoWrapper.getLanguageClientWrapper();
        
        if(!lcWrapper.isStarted())
            return;
        await lcWrapper.disposeLanguageClient(false);
    }

    async startLanguageClient()
    {
        await axios.get('/sanctum/csrf-cookie');
        await lcWrapper.start();
    }
    

    async restartLanguageClient()
    {
        const lcWrapper = this.monacoWrapper.getLanguageClientWrapper();
        
        if(!lcWrapper.isStarted())
            return;

        await axios.get('/sanctum/csrf-cookie');
        await lcWrapper.restartLanguageClient();
    }

    async onPreInit()
    {
        this.monacoWrapper = await runCppWrapper();
    }

    async onDestroy()
    {
        clearInterval(this.reconnectInterval);
        await this.monacoWrapper.dispose();
    }
    
    async onInit()
    {
        if(!this.monacoWrapper)
        {
            setTimeout(() => this.onInit(), 500);
            return;
        }
        
        try
        {
            await this.monacoWrapper.start(document.querySelector('.code-editor'));
        }
        catch(e)
        {
            // if we fail to connect/start the language client, let's skip the further reconenction attempts
            this.autoConnect = false;
        }
            
        let code = "";
        if(getStorageValue("code"))
        {
            code = getStorageValue("code");
        }
        else
        {
            code = examples.code1;
        }        
        
        // at this point we have a sane default value for code.

        // check if we're dealing with a share?
        if(this.sharedSlug)
        {
            await axios.get('/sanctum/csrf-cookie');
            try
            {
                const share = (await axios.get(`/api/share/${this.sharedSlug}`)).data;
                code = share.code;

                const keys = Object.keys(share.library_versions);
                keys.forEach((key) =>
                {
                    setStorageValue(key, share.library_versions[key]);
                });
                
                Cookies.set("pgetinker_libraries", encodeURIComponent(share.library_versions));
            }
            catch(err)
            {
                console.error(err);
            }
        }

        /**
         * don't stop... networking...
         * hold on to that linkaaaage
         * 
         * sockets.... streaming dataaaaa aahh ahhh
         */
        const reconnectHandler = async() =>
        {
            // language client is already started, let's not try, this time
            if(this.monacoWrapper.getLanguageClientWrapper().isStarted())
            {
                return;
            }
            
            clearInterval(this.reconnectInterval);
            
            try
            {
                await axios.get('/sanctum/csrf-cookie');
                await this.monacoWrapper.getLanguageClientWrapper().start();
            }
            catch(e)
            {
                // fail
            }
            
            this.reconnectInterval = setInterval(reconnectHandler, 5000);
        }
        
        if(this.autoConnect)
        {
            this.reconnectInterval = setInterval(reconnectHandler, 5000);
            reconnectHandler();
        }

        this.monacoWrapper.getEditor().setValue(code);

        this.monacoWrapper.getEditor().onDidChangeCursorPosition(() => this.updateStatusBar());
    
        this.monacoWrapper.getEditor().onDidChangeModelContent(() =>
        {
            setStorageValue("code", this.monacoWrapper.getEditor().getValue());
            
            if(this.sharedSlug)
            {
                this.sharedSlug = null;
                window.history.replaceState({}, "", "/");
            }
        });
        
        this.monacoWrapper.getEditor().updateOptions({
            glyphMargin: false,
        });
        
        /**
         * TODO: magic numbers are bad, mkay?
         * But I'm using them until they break!
         */
        this.monacoWrapper.getEditor().addAction({
            id: 'editor.action.build-and-run',
            label: 'Build and Run',
            keybindings: [
                2051, // monaco.KeyMod.CtrlCmd | monaco.KeyCode.Enter
                2097, // monaco.KeyMod.CtrlCmd | monaco.KeyCode.KeyS
            ],
            run: () =>
            {
                let startStopButton = document.querySelector("#start-stop");
                
                startStopButton.dispatchEvent(new Event("click"));
                
                // if we had to stop it first, click again!
                if(startStopButton.querySelector("span").innerHTML == "Run")
                    startStopButton.dispatchEvent(new Event("click"));
            }
        });

        this.updateStatusBar();
    }

    register()
    {
        this.state.layout.registerComponent('editorComponent', function(container)
        {
            container.getElement().html(`
                <div id="editor-panel">
                    <div class="code-editor"></div>
                    <div class="status">Loading</div>
                </div>
            `);
        });
    }
    
    exceedsMaxSize()
    {
        if(this.monacoWrapper == null)
            return false;

        return (this.monacoWrapper.getEditor().getValue().length > this.maxFileSize);
    }   
    
    focus()
    {
        if(this.monacoWrapper == null)
            return;

        this.monacoWrapper.getEditor().focus();
    }

    reveal(position)
    {
        if(this.monacoWrapper == null)
            return;

        this.monacoWrapper.getEditor().setPosition(position);
        this.focus();
        this.monacoWrapper.getEditor().revealPositionInCenter(position);
    }
    
    async updateConfiguration()
    {
        if(this.monacoWrapper == null)
            return;

        
        await this.monacoWrapper
            .getMonacoEditorApp()
            .updateUserConfiguration(getUserConfiguration());
    }

    updateStatusBar()
    {
        if(this.monacoWrapper == null)
            return;

        let statusBar = document.querySelector("#editor-panel .status");
    
        let cursor = `Ln ${this.monacoWrapper.getEditor().getPosition().lineNumber}, Col ${this.monacoWrapper.getEditor().getPosition().column}`;
        let fileSize = `${new Intl.NumberFormat().format(this.monacoWrapper.getEditor().getValue().length)} / ${new Intl.NumberFormat().format(this.maxFileSize)}`;
            
        statusBar.classList.toggle('too-fucking-big', false);
        if(this.monacoWrapper.getEditor().getValue().length > this.maxFileSize)
        {
            statusBar.classList.toggle('too-fucking-big', true);
            fileSize += " EXCEEDING MAXIMUM!";
        }
                
        statusBar.innerHTML = `
            <div class="status-left">
                Bytes: <span>${fileSize}</span>
            </div>
            <div class="status-right">
                <span>${cursor}</span>
            </div>
        `;
    }

}