import { createToast, ToastType } from './createToast';
import { getStorageValue, setStorageValue } from './storage';
import { button, resetFieldId, select, toggle } from "./form";
import Cookies from 'js-cookie';
import { getCompilerLibraries } from "./compilerLibraries";
import librariesDialog from './librariesDialog';

function renderCurrentLibraryVersions()
{
    const versions = getCompilerLibraries();

    const keys = Object.keys(versions);
    keys.sort((a, b) => a.localeCompare(b, undefined, { numeric: true, sensitivity: 'base' }));
    
    let output = `<div class="form-label">Current Library Versions</div>`;

    keys.forEach((key) =>
    {
        output += `<div class="library-version"><span>${key}</span><span>${versions[key]}</span></div>`;
    });

    return output;
}


export default function settingsDialog(state)
{
    
    return new Promise((resolve) =>
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
            "Restore Default Layout",
            async(event) =>
            {
                await state.switchToDefaultLayout();
                createToast("Restored default layout.", ToastType.Info);
            }
        ));
                
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
        

        dialog.querySelector(".content").append(button(
            "",
            renderCurrentLibraryVersions(),
            "Select Library Versions",
            (event) =>
            {
                event.preventDefault();
                dialog.remove();
                resolve();
            
                librariesDialog(state);
            }
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
        
        dialog.querySelector(".content").append(toggle(
            "Emscripten> Debug Mode",
            "<span><b>THIS SETTING DEGRADES RUNTIME PERFORMANCE!</b> Enabling this mode will instruct emscripten to generate debug symbols and to trace exceptions to make runtime debugging easier.</span>",
            (event) =>
            {
                setStorageValue("emscripten.debug", event.target.checked);
                state.editorPanel.updateStatusBar();
            },
            getStorageValue("emscripten.debug")
        ));

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
                dialog.remove();
                resolve();
            }
        }
        document.addEventListener('keydown', escapeKeyHandler);        
                
        document.body.appendChild(dialog);
    });
}
