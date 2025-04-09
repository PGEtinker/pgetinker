import { createToast, ToastType } from './createToast';
import { setStorageValue } from './storage';
import { resetFieldId, select } from "./form";
import Cookies from 'js-cookie';
import { getCompilerLibraries } from "./compilerLibraries";

export default function librariesDialog(state)
{
    let manifest;
    let currentLibraries;

    return new Promise(async(resolve, reject) =>
    {
        await axios.get('/sanctum/csrf-cookie');
        
        try
        {
            manifest = (await axios.get("/api/libraries")).data;
        }
        catch(err)
        {
            reject(err);
            return;
        }

        let dialog = document.createElement('div');
        
        dialog.classList.toggle("dialog", "true");
        dialog.classList.toggle("settings-dialog", "true");
                
        dialog.innerHTML = `
            <div class="window">
                <div class="header">Library Selector</div>
                <div class="content">
                </div>
                <div class="footer">
                    <button type="button" class="ok">Close</button>
                </div>                
            </div>`;

        dialog.addEventListener("update-bundle", (event) =>
        {
            event.preventDefault();
            
            resetFieldId();

            // ensure fresh versions
            currentLibraries = getCompilerLibraries();

            const pgeVersions = [];
            for(let i = 0; i < manifest.bundles["olcPixelGameEngine"].length; i++)
            {
                pgeVersions.push({
                    label: manifest.bundles["olcPixelGameEngine"][i].version,
                    value: manifest.bundles["olcPixelGameEngine"][i].version
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
                pgeVersions,
                currentLibraries["olcPixelGameEngine"],
            ));
                
            for(let i = 0; i < manifest.bundles["olcPixelGameEngine"].length; i++)
            {
                if(manifest.bundles["olcPixelGameEngine"][i].version != currentLibraries["olcPixelGameEngine"])
                    continue;
                
                const currentBundle = manifest.bundles["olcPixelGameEngine"][i];

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

        dialog.querySelector(".ok").addEventListener("click", (event) =>
        {
            dialog.remove();
            resolve();
        });
        
        dialog.dispatchEvent(new Event("update-bundle"));
                
        document.body.appendChild(dialog);
    });
}
