import { getCompilerLibraries } from './compilerLibraries';
import { createToast, ToastType } from './createToast';
import librariesDialog from './librariesDialog';
import { setStorageValue } from './storage';
import Cookies from 'js-cookie';

export default function examplesDialog(state)
{
    return new Promise((resolve) =>
    {
        let dialog = document.createElement("div");
    
        dialog.classList.toggle("dialog", true);
        dialog.classList.toggle("examples", true);
        
        dialog.innerHTML = `
        <div class="window">
            <div class="header">Examples</div>
            <div class="content">
                <div class="search-bar">
                    <input type="text" id="search" placeholder="Search examples by name, function, or keyword..." aria-label="Search examples by keyword or name">
                    <div class="search-filters">
                        Filters:
                        <input id="show-pge" type="checkbox" checked> olcPixelGameEngine
                        <input id="show-raylib" type="checkbox" checked> raylib
                    </div>
                </div>
                <div class="examples-container"></div>
            </div>
            <div class="footer">
                <button class="ok">Close</button>
            </div>
        </div>`;

        const elem_examples = dialog.querySelector(".examples-container");
        const elem_search = dialog.querySelector("#search");
        const elem_showPGE = dialog.querySelector("#show-pge");
        const elem_showRaylib = dialog.querySelector("#show-raylib");

        function renderExamples(examples, filter = "", showPGE, showRaylib)
        {
            elem_examples.innerHTML = "";
            
            const searchTerm = filter.toLowerCase();
            let haveMatches = false;
            examples.forEach((example) =>
            {
                if(!showPGE && example.library === "pge")
                    return;
                
                if(!showRaylib && example.library === "raylib")
                    return;
                
                const matchKeywords = example.keywords.toLowerCase().includes(searchTerm);
                const matchName     = example.name.toLowerCase().includes(searchTerm);
                
                if (matchKeywords || matchName )
                {
                    haveMatches = true;
                    const card = document.createElement("div");
                    
                    card.className = "example";
                    
                    card.innerHTML = `
                        <h3>${example.name}</h3>
                        <div class="screenshot">
                            <img src="${example.screenshot}" alt="${example.name} screenshot">
                            <img src="/images/${example.library}.png" alt="${example.library} logo">
                        </div>
                    `;

                    card.addEventListener("click", (event) =>
                    {
                        event.preventDefault();
                        createToast(`Loading Example: ${example.name}`, ToastType.Success);
                        fetch(example.codeFile)
                            .then((response) => response.text())
                            .then(async(code) =>
                            {
                                const libKeys = Object.keys(librariesManifest.latest);
                                libKeys.forEach((key) =>
                                {
                                    setStorageValue(key, librariesManifest.latest[key]);
                                });
                                Cookies.set("pgetinker_libraries", encodeURIComponent(JSON.stringify(getCompilerLibraries())));
                                
                                state.editorPanel.setValue("");
                                
                                setTimeout(() =>
                                {
                                    state.editorPanel.setValue(code);
                                    state.editorPanel.reveal({ column: 1, lineNumber: 1 });
                                    
                                    setTimeout(async() =>
                                    {
                                        await state.editorPanel.restartLanguageClient()
                                        dialog.remove();
                                        resolve();
                                    }, 50);
                                }, 50);
                            });
                    });

                    elem_examples.appendChild(card);
                }
            });
            
            if(!haveMatches)
            {
                elem_examples.innerHTML = `<h3>No Results</h3>`;
            }
        }

        fetch("/examples/examples.json")
            .then((result) => result.json())
            .then((data) =>
            {
                // Initial render
                renderExamples(data, "", elem_showPGE.checked, elem_showRaylib.checked);

                // Search event listener
                elem_search.addEventListener("input", e => {
                    renderExamples(data, e.target.value, elem_showPGE.checked, elem_showRaylib.checked);
                });

                elem_showPGE.addEventListener("input", (e) =>
                {
                    renderExamples(data, elem_search.value, elem_showPGE.checked, elem_showRaylib.checked);
                });

                elem_showRaylib.addEventListener("input", (e) =>
                {
                    renderExamples(data, elem_search.value, elem_showPGE.checked, elem_showRaylib.checked);
                });
            })
            .catch((reason) =>
            {
                console.error(reason);
            });
        
 
        dialog.querySelector("button.ok").addEventListener("click", (event) =>
        {
            dialog.remove();
            resolve();
        });

        function escapeKeyHandler(event)
        {
            if(event.key === 'Escape')
            {
                document.removeEventListener("keydown", escapeKeyHandler);
                dialog.remove();
                resolve();
            }
        }
        document.addEventListener('keydown', escapeKeyHandler);

        document.body.appendChild(dialog);
    });

}