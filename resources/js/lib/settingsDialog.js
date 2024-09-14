import { createToast, ToastType } from './createToast';
import { getStorageValue, setStorageValue } from './storage';
import { button, resetFieldId, select, toggle } from "./form";


function renderVersionSelector(elem, bundleData)
{
    let bundle = null;
    
    let pgeOptions = [];
    for(let i = 0; i < bundleData.olcPixelGameEngine.length; i++)
    {
        if(bundleData.olcPixelGameEngine[i].version === getStorageValue("olcPixelGameEngine"))
        {
            bundle = bundleData.olcPixelGameEngine[i];
        }
        pgeOptions.push({
            label: bundleData.olcPixelGameEngine[i].version,
            value: bundleData.olcPixelGameEngine[i].version
        })
    }
    
    if(bundle === null)
        return;

    elem.append(select(
        "olcPixelGameEngine",
        "Choose the version of olcPixelGameEngine to use.",
        (event) =>
        {
            createToast(`Switched olcPixelGameEngine to version: ${event.target.value}.`, ToastType.Info);
            setStorageValue("olcPixelGameEngine", event.target.value);
            setTimeout(() =>
            {
                elem.innerHTML = "";
                renderVersionSelector(elem, bundleData);
            }, 50);
        },        
        pgeOptions,
        bundle.version
    ));

    bundle.libraries.forEach((library) =>
    {
        elem.append(select(
            library,
            `Choose the version of ${library} to use.`,
            (event) =>
            {
                setStorageValue(library, event.target.value);
                createToast(`Switched ${library} to version: ${event.target.value}.`, ToastType.Info);
            },
            bundle[library].map(version => ({ label: version, value: version })),
            getStorageValue(library)
        ));
    });
}

export default function settingsDialog(state)
{
    
    return new Promise((resolve) =>
    {
        axios.get('/sanctum/csrf-cookie').then(_ =>
        {
            axios.get("/api/libraries").then((response) =>
            {
                resetFieldId();

                let dialog = document.createElement('div');
                
                dialog.classList.toggle("dialog", "true");
                dialog.classList.toggle("settings-dialog", "true");
                
                dialog.innerHTML = `
                    <div class="window">
                        <div class="header">Settings</div>
                        <div class="content">
                        </div>
                        <div class="footer">
                            <button type="button" class="ok">Close</button>
                        </div>                
                    </div>`;
                
                dialog.querySelector(".content").append(button(
                    "Restore Default Layout",
                    "If you're unhappy with the layout and want to just start over!",
                    async(event) =>
                    {
                        await state.switchToDefaultLayout();
                        createToast("Restored default layout.", ToastType.Info);
                    }
                ));
                
                const elem_versionSelector = document.createElement("div");
                dialog.querySelector(".content").append(elem_versionSelector);
                renderVersionSelector(elem_versionSelector, response.data.bundles);

                dialog.querySelector(".content").append(select(
                    "Theme",
                    "Choose from the available themes!",
                    (event) =>
                    {
                        if(event.target.value === "dark")
                            state.theme = "dark";
        
                        if(event.target.value === "light")
                            state.theme = "light";
                            
                        createToast(`Changing theme: ${state.theme}`, ToastType.Info);
                        setStorageValue("theme", state.theme);
        
                        state.UpdateTheme();
                    },
                    [
                        {
                            label: "Dark Theme",
                            value: "dark",
                        },
                        {
                            label: "Light Theme",
                            value: "light",
                        }
                    ],
                    getStorageValue("theme")
                ));
        
                dialog.querySelector(".content").append(toggle(
                    "Editor> Inlay Hints: Enabled",
                    "Enables the inlay hints in the editor.",
                    (event) =>
                    {
                        setStorageValue("editor.inlayHints.enabled", event.target.checked);
                        state.editorPanel.updateConfiguration();
                    },
                    getStorageValue("editor.inlayHints.enabled")
                ));
        
                dialog.querySelector(".content").append(toggle(
                    "Diagnostics> Javid Mode: Enabled",
                    "Enabling Javid Mode prevents Clang Tidy warnings from appearing in the editor and the problems panel.",
                    (event) =>
                    {
                        setStorageValue("diagnostics.javidMode", event.target.checked);
                        if(event.target.checked)
                        {
                            window.dispatchEvent(new CustomEvent("update-problems-panel", { detail: [] }));
                        }
                    },
                    getStorageValue("diagnostics.javidMode")
                ));
        
                dialog.querySelector(".ok").addEventListener("click", (event) =>
                {
                    dialog.remove();
                    resolve();
                });
                
                document.body.appendChild(dialog);
            });
        });

    

    });
}
