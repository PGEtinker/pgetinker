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

        dialog.addEventListener("update-bundle", (event) =>
        {
            event.preventDefault();
            
            resetFieldId();

            // ensure fresh versions
            let currentLibraries = getCompilerLibraries();

            const options = [];
            for(let i = 0; i < librariesManifest.bundles["olcPixelGameEngine"].length; i++)
            {
                options.push({
                    label: librariesManifest.bundles["olcPixelGameEngine"][i].version,
                    value: librariesManifest.bundles["olcPixelGameEngine"][i].version
                });
            }

            dialog.querySelector(".content").innerHTML = "";

            dialog.querySelector(".content").append(select(
                "olcPixelGameEngine",
                "Choose from the available versions of olcPixelGameEngine",
                (event) =>
                {
                    // TODO: sanity check this
                    setStorageValue("olcPixelGameEngine", event.target.value);
                    createToast(`Changing olcPixelGameEngine to : ${event.target.value}`, ToastType.Info);
                    Cookies.set("pgetinker_libraries", encodeURIComponent(JSON.stringify(getCompilerLibraries())));
                    state.editorPanel.restartLanguageClient();

                    dialog.dispatchEvent(new Event("update-bundle"));
                },
                options,
                currentLibraries["olcPixelGameEngine"],
            ));
                
            for(let i = 0; i < librariesManifest.bundles["olcPixelGameEngine"].length; i++)
            {
                if(librariesManifest.bundles["olcPixelGameEngine"][i].version != currentLibraries["olcPixelGameEngine"])
                    continue;
                
                const currentBundle = librariesManifest.bundles["olcPixelGameEngine"][i];

                for(let i = 0; i < currentBundle.libraries.length; i++)
                {
                    const options = [];
                    
                    currentBundle[currentBundle.libraries[i]].forEach((item) =>
                    {
                        options.push({
                            label: item,
                            value: item
                        });
                    });
            
                    dialog.querySelector(".content").append(select(
                        currentBundle.libraries[i],
                        `Choose from the available versions of ${currentBundle.libraries[i]}`,
                        (event) =>
                        {
                            setStorageValue(currentBundle.libraries[i], event.target.value);
                            createToast(`Changing ${currentBundle.libraries[i]} to : ${event.target.value}`, ToastType.Info);
                            Cookies.set("pgetinker_libraries", encodeURIComponent(JSON.stringify(getCompilerLibraries())));
                            state.editorPanel.restartLanguageClient();
                        },
                        options,
                        currentLibraries[currentBundle.libraries[i]],
                    ));
                }
            }
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
        
        dialog.dispatchEvent(new Event("update-bundle"));

        document.body.appendChild(dialog);
    });
}
