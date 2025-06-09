import { createToast, ToastType } from './createToast';
import { setStorageValue } from './storage';
import { resetFieldId, select } from "./form";
import Cookies from 'js-cookie';
import { getCompilerLibraries } from "./compilerLibraries";
import settingsDialog from './settingsDialog';

export default function librariesDialog(state)
{
    return new Promise((resolve) =>
    {
        let dialog = document.createElement('div');
        
        dialog.classList.toggle("dialog", "true");
        dialog.classList.toggle("settings-dialog", "true");
                
        dialog.innerHTML = `
            <div class="window">
                <div class="header">Library Selector</div>
                <div class="content">
                </div>
                <div class="footer">
                    <button type="button" class="back">Back</button>
                    <button type="button" class="ok">Close</button>
                </div>                
            </div>`;

        const libraries = Object.keys(librariesManifest.libraries);
        
        resetFieldId();

        // ensure fresh versions
        let currentLibraries = getCompilerLibraries();

        libraries.forEach((library) =>
        {
            const options = [];

            librariesManifest.libraries[library].forEach((version) =>
            {
                options.push({
                    label: version,
                    value: version
                });                
            });

            dialog.querySelector(".content").append(select(
                library,
                `Choose from the available versions of ${library}`,
                (event) =>
                {
                    // TODO: sanity check this
                    setStorageValue(library, event.target.value);
                    createToast(`Changing ${library} to : ${event.target.value}`, ToastType.Info);
                    Cookies.set("pgetinker_libraries", encodeURIComponent(JSON.stringify(getCompilerLibraries())));
                    state.editorPanel.restartLanguageClient();
                },
                options,
                currentLibraries[library],
            ));

        });

        dialog.querySelector(".back").addEventListener("click", (event) =>
        {
            settingsDialog(state);
            dialog.remove();
            resolve();
        });
    
        dialog.querySelector(".ok").addEventListener("click", (event) =>
        {
            dialog.remove();
            resolve();
        });

        function escapeKeyHandler(event)
        {
            if(event.key === 'Escape')
            {
                document.removeEventListener("keydown", escapeKeyHandler);
                settingsDialog(state);
                dialog.remove();
                resolve();
            }
        }
        document.addEventListener('keydown', escapeKeyHandler);        

        document.body.appendChild(dialog);
    });
}
