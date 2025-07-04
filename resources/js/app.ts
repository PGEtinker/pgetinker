import './lib/bootstrap';
import { getStorageValue, setStorageValue } from './lib/storage';
import './lib/goldenLayout';

import version from "./lib/version";
// @ts-ignore
import examplesDialog from './lib/examplesDialog';
// @ts-ignore
import mobileMenuDialog from './lib/mobileMenuDialog';
// @ts-ignore
import newsDialog from './lib/newsDialog';
// @ts-ignore
import settingsDialog from './lib/settingsDialog';
// @ts-ignore
import shareDialog from './lib/shareDialog';
// @ts-ignore
import supportersDialog from './lib/supportersDialog';
// @ts-ignore
import defaultLandscapeLayout from './lib/defaultLandscapeLayout';
// @ts-ignore
import defaultPortraitLayout from './lib/defaultPortraitLayout';
// @ts-ignore
import ConsolePanel from './components/ConsolePanel';
// @ts-ignore
import CompilerOutputPanel from './components/CompilerOutputPanel';
// @ts-ignore
import EditorPanel from './components/EditorPanel';
// @ts-ignore
import PlayerPanel from './components/PlayerPanel';
// @ts-ignore
import ProblemsPanel from './components/ProblemsPanel';
import axios from 'axios';
import { createToast, ToastType } from './lib/createToast';
import { getCompilerLibraries } from './lib/compilerLibraries';

declare function GoldenLayout(...args: any[]): void;

export default class PGEtinker
{
    consolePanel;
    editorPanel;
    compilerOutputPanel;
    playerPanel;
    problemsPanel;

    layoutInitialized = false;
    compiling = false;
    layout: any;
    layoutConfig: any = null;
    
    theme: string | null = "dark";

    constructor()
    {
        this.consolePanel        = new ConsolePanel(this);
        this.compilerOutputPanel = new CompilerOutputPanel(this);
        this.editorPanel         = new EditorPanel(this);
        this.playerPanel         = new PlayerPanel(this);
        this.problemsPanel       = new ProblemsPanel(this);
        
        this.layoutConfig = getStorageValue("layout");
        
        if(this.layoutConfig === null)
        {
            this.layoutConfig = defaultLandscapeLayout;
            
            if(document.body.clientWidth <= 750)
            {
                console.log("chose portrait layout");
                this.layoutConfig = defaultPortraitLayout;
            }
            else
            {
                console.log("chose landscapre layout");
            }
        }
        
        document.querySelector("#settings-menu")?.addEventListener("click", (event) =>
        {
            event.preventDefault();
            if(this.isLoading())
                return;

            if(document.body.clientWidth <= 750)
            {
                mobileMenuDialog(this);
                return;
            }
            settingsDialog(this);
        });
        
        document.querySelector("#examples-menu")?.addEventListener("click", (event) =>
        {
            event.preventDefault();
            if(this.isLoading())
                return;

            examplesDialog(this);
        });

        // Download Button
        document.querySelector("#download")?.addEventListener("click", (event) => 
        {
            event.preventDefault();
            if(this.isLoading())
                return;

            this.download();            
        });
        
        // Share Button
        document.querySelector("#share")!.addEventListener("click", (event) => 
        {
            event.preventDefault();
            if(this.isLoading())
                return;

            this.share();
        });

        // Compile Button
        document.querySelector("#start-stop")!.addEventListener("click", (event) => 
        {
            event.preventDefault();
            if(this.isLoading())
                return

            let startStopElem = document.querySelector("#start-stop")!;
            let playIconElem = startStopElem.querySelector(".lucide-circle-play")!;
            let stopIconElem = startStopElem.querySelector(".lucide-circle-stop")!;
            let spanElem     = startStopElem.querySelector("span")!;

            if(spanElem.innerHTML == "Run")
            {
                this.setActiveTab("player");
                
                this.consolePanel.setFirstRun();
                playIconElem.classList.toggle("hidden", true);
                stopIconElem.classList.toggle("hidden", false);
                spanElem.innerHTML = "Stop";
                this.compile().catch(() =>
                {
                    playIconElem.classList.toggle("hidden", false);
                    stopIconElem.classList.toggle("hidden", true);
                    spanElem.innerHTML = "Run";
                });
                return;
            }

            if(spanElem.innerHTML == "Stop")
            {
                this.setActiveTab("editor");

                this.playerPanel.stop();
                playIconElem.classList.toggle("hidden", false);
                stopIconElem.classList.toggle("hidden", true);
                spanElem.innerHTML = "Run";
            }
        });

        document.querySelector("#supporters")!.addEventListener("click", (event) =>
        {
            event.preventDefault();
            if(this.isLoading())
                return;
            
            supportersDialog();
        });

        document.querySelector("#news-and-updates")!.addEventListener("click", (event) =>
        {
            event.preventDefault();
            if(this.isLoading())
                return;

            newsDialog();
        });

        this.SetupLayout();
        this.setActiveTab("editor");
        this.setActiveTab("problems");

        // Create a media query for prefers-color-scheme
        const darkModeMediaQuery = window.matchMedia('(prefers-color-scheme: dark)');

        // Function to handle theme change
        const handleThemeChange = (e: any) =>
        {
            if (e.matches) {
                setStorageValue("theme", "dark");
                console.log('Dark mode is active');
                // Apply dark mode styles or logic
            } else {
                setStorageValue("theme", "light");
                console.log('Light mode is active');
                // Apply light mode styles or logic
            }
            
            this.UpdateTheme();
        }

        // Add event listener for theme changes
        darkModeMediaQuery.addEventListener('change', handleThemeChange);
    }

    setActiveTab(id: string)
    {
        try
        {
            let panel = this.layout.root.getItemsByFilter((item: any) =>
            {
                return (item.config.id == id);
            })[0];
            
            if(panel.parent.isStack)
            {
                panel.parent.setActiveContentItem(panel);
            }
        }
        catch(e)
        {
            console.log(`Failed to setActiveTab("${id}")`);
        }
    }
    
    download()
    {
        if(!this.playerPanel.getHtml().includes("Emscripten-Generated Code"))
        {
            createToast("You have to build the code before you can download!", ToastType.Danger, 10000);
            return;
        }
        
        const a = document.createElement('a');
        
        // create the data url
        a.href = `data:text/html;base64,${btoa(this.playerPanel.getHtml())}`;
        a.download = "pgetinker.html";

        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        createToast("Downloading HTML.", ToastType.Info);
    }
    
    share()
    {
        let startStopElem = document.querySelector("#start-stop")!;
        let playIconElem = startStopElem.querySelector(".lucide-circle-play")!;
        let stopIconElem = startStopElem.querySelector(".lucide-circle-stop")!;
        let spanElem     = startStopElem.querySelector("span")!;

        this.setActiveTab("player");
                
        this.consolePanel.setFirstRun();
        playIconElem.classList.toggle("hidden", true);
        stopIconElem.classList.toggle("hidden", false);
        spanElem.innerHTML = "Stop";

        if(this.compiling)
            return;

        if(!this.preCompile())
            return;
        
        axios.get('/sanctum/csrf-cookie').then(_ => {
            axios.post("/api/share", {
                code: this.editorPanel.getValue(),
                libraries: getCompilerLibraries()
            }).then((response) =>
            {
                this.compileSuccessHandler(response.data);
                
                shareDialog(response.data.shareURL, response.data.shareThumbURL)
                    .finally(() =>
                    {
                        
                    });
            
            }).catch((error) =>
            {
                this.setActiveTab("editor");
                playIconElem.classList.toggle("hidden", false);
                stopIconElem.classList.toggle("hidden", true);
                spanElem.innerHTML = "Run";
        
    
                if(error.response)
                {
                    if(error.response.status)
                    {
                        if(error.response.status == 503)
                        {
                            this.compileFailHandler("pgetinker.cpp:1:1: error: PGEtinker service has gone offline. try again later.\n");
                            return;
                        }
                    }
    
                    if(error.response.data.stderr)
                    {
                        this.compileFailHandler(error.response.data.stderr);
                        return;
                    }
                }
                
                this.compileFailHandler("pgetinker.cpp:1:1: error: compilation failed in a way that's not being handled. please make a bug report.\n");
            });
        }).catch(() =>
        {
            this.compileFailHandler("pgetinker.cpp:1:1: error: calling the compiler api failed.\n");
        });
    }

    async switchToDefaultLayout()
    {
        await this.editorPanel.onDestroy();

        this.layout.destroy();

        this.layoutConfig = defaultLandscapeLayout;
        
        if(document.body.clientWidth <= 750)
        {
            console.log("chose portrait layout");
            this.layoutConfig = defaultPortraitLayout;
        }
        
        this.SetupLayout();

    }

    preCompile()
    {
        if(this.editorPanel.exceedsMaxSize())
        {
            alert("Maximum size exceeded!");
            return false;
        }
        
        this.compilerOutputPanel.focus();
        this.compilerOutputPanel.clear();
        this.consolePanel.clear();

        this.playerPanel.setCompiling();
        
        this.compiling = true;
        return true;
    }
    
    compile()
    {
        if(this.compiling)
            return new Promise<void>((_, reject) => reject());

        if(!this.preCompile())
            return new Promise<void>((_, reject) => reject());
        
        return new Promise<void>((resolve, reject) =>
        {
            axios.get('/sanctum/csrf-cookie').then(_ => {
                
                axios.post("/api/compile", {
                    code: this.editorPanel.getValue(),
                    libraries: getCompilerLibraries()
                }).then((response) =>
                {
                    this.compileSuccessHandler(response.data);
                    resolve();
                }).catch((error) =>
                {
                    
                    if(error.response)
                    {
                        if(error.response.status)
                        {
                            if(error.response.status == 503)
                            {
                                this.compileFailHandler("pgetinker.cpp:1:1: error: PGEtinker service has gone offline. try again later.\n");
                                reject();
                                return;
                            }
                        }
                        
                        if(error.response.data.stderr)
                        {
                            this.compileFailHandler(error.response.data.stderr);
                            reject();
                            return;
                        }
                    }
                    this.compileFailHandler("pgetinker.cpp:1:1: error: compilation failed in a way that's not being handled. please make a bug report.\n");
                    reject();
                });
            }).catch(() =>
            {
                this.compileFailHandler("pgetinker.cpp:1:1: error: calling the compiler api failed.\n");
                reject();
            });

        });
    }
    
    compileSuccessHandler(data: any)
    {
        this.compilerOutputPanel.setContent(data.stdout + data.stderr);
        this.playerPanel.setHtml(data.html);
        this.compiling = false;
        if(document.body.clientWidth <= 750)
        {
            createToast("Compile Success.", ToastType.Success);
        }
    }
    
    compileFailHandler(stderr: any)
    {
        this.setActiveTab("editor");

        this.compilerOutputPanel.setContent(stderr);
        this.playerPanel.setCompilingFailed();
        this.compiling = false;
        if(document.body.clientWidth <= 750)
        {
            createToast("Compile Failed.", ToastType.Danger);
        }
    }

    async SetupLayout()
    {
        await this.editorPanel.onPreInit();
        
        // @ts-ignore
        this.layout = new GoldenLayout(this.layoutConfig, document.querySelector("#content"))
        
        this.consolePanel.register();
        this.editorPanel.register();
        this.compilerOutputPanel.register();
        this.playerPanel.register();
        this.problemsPanel.register();

        this.layout.on("stateChanged", () =>
        {
            if(this.layoutInitialized)
            {
                setStorageValue("layout", this.layout.toConfig());
            }
        });
        this.layout.on("initialised", async() =>
        {
            this.layoutInitialized = true;
            window.addEventListener("resize", () =>
            {
                this.layout.updateSize();
            });

            if(!this.playerPanel.isRunning())
            {
                this.playerPanel.lastPlayerHtml = "";
            }
            
            
            this.consolePanel.onInit();
            await this.editorPanel.onInit();
            this.compilerOutputPanel.onInit();
            this.playerPanel.onInit();
            this.problemsPanel.onInit();

            await this.UpdateTheme();
            
            setTimeout(() =>
            {
                this.setActiveTab("editor");
            }, 500)

            document.querySelector("#pgetinker-loading")!.classList.toggle("hidden", false);
            setTimeout(() =>
            {
                document.querySelectorAll("#header li.hidden")!.forEach((item) =>
                {
                    item.classList.toggle("hidden", false);
                });
            }, 100);
        });
    
        this.layout.init();

        if(version !== getStorageValue("version"))
        {
            newsDialog()
                .finally(() =>
                {
                    setStorageValue("version", version);
                });
        }
    }
    
    async UpdateTheme()
    {
        const theme = getStorageValue("theme");
        let light = (theme === "light");

        // update editor theme
        await this.editorPanel.updateConfiguration();
        
        // 
        const isVsCodeTheme = () =>
        {
            return document.body.classList.contains(`vscode-theme-defaults-themes-${theme}_modern-json`);
        }
        
        let updateInterval : NodeJS.Timeout;

        const updatehandler = () =>
        {
            if(isVsCodeTheme())
            {
                clearInterval(updateInterval);

                document.body.classList.toggle("dark", !light);
                document.body.classList.toggle("light", light);
        
                // update golden layout theme
                let goldenLayoutDarkThemeStyle = document.querySelector("#goldenlayout-dark-theme")! as HTMLLinkElement;
                let goldenLayoutLightThemeStyle = document.querySelector("#goldenlayout-light-theme")! as HTMLLinkElement;
            
                goldenLayoutDarkThemeStyle.disabled = light;
                goldenLayoutLightThemeStyle.disabled = !light;
            
                // update player theme
                this.playerPanel.setTheme(theme);
                
                document.querySelector("#pgetinker-loading")!.classList.toggle("hidden", true);
                return;
            }

        }

        updateInterval = setInterval(updatehandler, 50);
    }

    isLoading()
    {
        return !document.querySelector("#pgetinker-loading")!.classList.contains("hidden");
    }
}

