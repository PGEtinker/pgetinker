import { getUserConfiguration } from "../lib/monacoConfig";
import { runCppWrapper } from "../lib/monacoWrapper";
import pgetinkerCppCode from '../../example.cpp?raw';

export default class EditorPanel
{
    state;
    
    code = "";

    monacoWrapper = null;

    maxFileSize = 50000;
    
    sharedFlag = false;

    constructor(state)
    {
        this.state = state;
        this.sharedFlag = (window.location.pathname.indexOf("/s/") === 0);
    }
    
    getValue()
    {
        return this.monacoWrapper.getEditor().getValue();
    }
    
    setValue(value)
    {
        this.monacoWrapper.getEditor().setValue(value);
    }
    
    async onPreInit()
    {
        try
        {
            if(this.monacoWrapper !== null)
            {
                await this.monacoWrapper.dispose();
            }
        }
        catch(e)
        {
            console.log(e);
        }
    }

    async onInit()
    {
        this.monacoWrapper = await runCppWrapper(document.querySelector(".code-editor"));
            
        let code = "";
        if(this.sharedFlag)
        {
            code = document.querySelector('#code').value;
        }
        else if(window.localStorage.getItem("pgetinkerCode"))
        {
            code = JSON.parse(window.localStorage.getItem("pgetinkerCode"));
        }
        else
        {
            code = pgetinkerCppCode;
        }

        this.monacoWrapper.getEditor().setValue(code);

        this.monacoWrapper.getEditor().onDidChangeCursorPosition(() => this.updateStatusBar());
    
        this.monacoWrapper.getEditor().onDidChangeModelContent(() =>
        {
            window.localStorage.setItem("pgetinkerCode", JSON.stringify(this.monacoWrapper.getEditor().getValue()));
            
            if(this.sharedFlag)
            {
                window.history.replaceState({}, "", "/");
            }
        });
            
        this.monacoWrapper.getEditor().addAction({
            id: 'editor.action.build-and-run',
            label: 'Build and Run',
            keybindings: [],
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
        return (this.monacoWrapper.getEditor().getValue().length > this.maxFileSize);
    }   

    reveal(position)
    {
        this.monacoWrapper.getEditor().revealPositionInCenter(position);
    }
    
    extractAndSetMarkers(input)
    {
    }

    clearMarkers()
    {
    }

    setMarkers(markers)
    {
        // // set model markers
        // monaco.editor.setModelMarkers(this.monacoModel, "owner", markers);
        // // move to first marker
        // this.monacoEditor.setPosition({lineNumber: markers[0].startLineNumber, column: markers[0].startColumn });
        // // trigger activate nearest marker
        // setTimeout(() => { this.monacoEditor.trigger("", "editor.action.marker.next"); }, 50);
    }

    setTheme(theme)
    {
        if(this.monacoWrapper == null)
            return;

        
        this.monacoWrapper
            .getMonacoEditorApp()
            .updateUserConfiguration(getUserConfiguration(theme));
    }

    updateStatusBar()
    {
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