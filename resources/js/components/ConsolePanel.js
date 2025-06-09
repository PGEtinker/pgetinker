import { createToast, ToastType } from '../lib/createToast';

function setupLogger(consolePanel)
{
    //-----------------------------------------------------------------
    // Private Variables
    //-----------------------------------------------------------------
    const element = consolePanel.getElement();

    const chunks  = [];
    const working = [];

    const queue   = [];

    let autoScroll   = true;
    let currentChunk = 0;
    let interval     = null;

    //-----------------------------------------------------------------
    // Private Methods
    //-----------------------------------------------------------------
    function reset()
    {
        stop();

        chunks.length  = 0;
        working.length = 0;

        queue.length   = 0;

        autoScroll     = true;
        currentChunk   = 0;
        interval       = null;

        start();
    }

    function isAtBottom()
    {
        return element.scrollTop + element.clientHeight >= element.scrollHeight - 1;
    }

    function isNearTop()
    {
        return element.scrollTop < 10;
    }

    function queueHandler()
    {

        for(let i = 0; i < queue.length; i++)
        {
            if(working.length == 100)
            {
                chunks.push(structuredClone(working));
                working.length = 0;
            }

            working.push(queue[i]);
        }
        
        queue.length = 0;
        
        if(!autoScroll)
            return;

        currentChunk = chunks.length - 1;

        if(chunks.length == 0)
        {
            element.innerHTML = `<div>${working.join("")}</div>`;
        }
        else
        {
            element.innerHTML = `<div>${chunks[chunks.length -1].join("")}</div><div>${working.join("")}</div>`;
        }
        
        element.scrollTop = element.scrollHeight;
    };

    function handleManualScrolling(event)
    {
        const isUp = !!(
            ((event.key) && (event.key == "PageUp")) ||
            ((event.key) && (event.key == "ArrowUp")) ||
            ((event.deltaY) && (event.deltaY < 0))
        );
        
        const isDown = !!(
            ((event.key) && (event.key == "PageDown")) ||
            ((event.key) && (event.key == "ArrowDown")) ||
            ((event.deltaY) && (event.deltaY > 0))
        );
        
        if(isUp)
        {
            if(isNearTop() && currentChunk != 0)
            {
                currentChunk--;
                
                const children = element.querySelectorAll("div");
                
                const chunkElem = document.createElement("div");
                chunkElem.innerHTML = chunks[currentChunk].join("");
                element.prepend(chunkElem);
                children[0].scrollIntoView();

                if(children.length > 1)
                    children[1].remove();
            }
            return;
        }

        if(isDown)
        {
            if(isAtBottom())
            {
                if(currentChunk == chunks.length - 1)
                {
                    autoScroll = true;
                    return;
                }
                
                currentChunk++;

                if(currentChunk <= chunks.length - 2)
                {
                    const children = element.querySelectorAll("div");
                    const chunkElem = document.createElement("div");
                    chunkElem.innerHTML = chunks[currentChunk + 1].join("");
                    element.append(chunkElem);

                    if(children.length > 1)
                        children[0].remove();
                }
            }
        }
    }

    //-----------------------------------------------------------------
    // Public Methods
    //-----------------------------------------------------------------

    function addEntry(text)
    {
        queue.push(text);
    }
    
    function clear()
    {
        console.log("LOGGER:", "clear()");
        reset();
    }
    
    function resumeAutoScroll()
    {
        autoScroll = true;
    }

    function start()
    {
        console.log("LOGGER:", "start()");
        if(interval)
            return;

        interval = setInterval(queueHandler, 100);
    }

    function stop()
    {
        console.log("LOGGER:", "stop()");
        if(!interval)
            return;
        
        clearInterval(interval);
        interval = null;
    }
    
    function toString()
    {
        console.log("LOGGER:", "toString()");
        stop();
        const output = chunks.map(chunk => chunk.join("")).join("") + working.join("") + queue.join("");
        start();
        return output;
    }

    //-----------------------------------------------------------------
    // Event Handlers
    //-----------------------------------------------------------------
    element.addEventListener("mousewheel", handleManualScrolling);
    element.addEventListener("keydown", handleManualScrolling);
    element.addEventListener("scroll", (event) =>
    {
        if(currentChunk == 0)
            return;
        
        autoScroll = isAtBottom() && (currentChunk == chunks.length - 1);
        document.querySelector("#console-buttons").classList.toggle("hidden", autoScroll);
    });
    
    element.addEventListener('mouseenter', () => element.focus());
    element.addEventListener('mouseleave', () => element.blur());

    return {
        addEntry,
        clear,
        resumeAutoScroll,
        start,
        stop,
        toString,
    };
}


export default class ConsolePanel
{
    state;
    firstRun;
    
    logger;
    
    constructor(state)
    {
        this.state = state;
        this.firstRun = true;
        
        console.log("Console panel", "constructor");
        
        window.addEventListener("message", (event) =>
        {
            if(typeof event.data !== "object")
                return;
                
            if(typeof event.data.message !== "string")
                return;

            if(event.data.message === "console-output")
            {
                if(this.firstRun)
                {
                    this.state.setActiveTab("console");
                    this.firstRun = false;
                }
                
                this.logger.addEntry(event.data.data);
                return;
            }
        });
    }
    
    clear()
    {
        this.firstRun = true;
        this.logger.clear();
    }

    getElement()
    {
        return document.querySelector('#console-panel');
    }

    onInit()
    {
        document.querySelector("#console-download").addEventListener("click", () =>
        {
            this.download();
        });
        
        document.querySelector("#console-auto-scroll").addEventListener("click", () =>
        {
            this.logger.resumeAutoScroll();
        });

        this.logger = setupLogger(this);
    }

    register()
    {
        this.state.layout.registerComponent('consoleComponent', function(container)
        {
            container.getElement().html(`
                <div id="console-panel" tabindex="0"></div>
                <div id="console-buttons" class="hidden">
                    <button id="console-auto-scroll">AutoScroll</button>
                    <button id="console-download">Download Log</button>
                </div>
            `);
        });
    }

    setFirstRun()
    {
        this.firstRun = true;
    }

    download()
    {
        const a = document.createElement('a');
        
        // create the data url
        a.href = `data:text/plain;base64,${btoa(this.logger.toString())}`;
        a.download = "pgetinker-log.txt";

        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        createToast("Downloading Log.", ToastType.Info);
    }
}
