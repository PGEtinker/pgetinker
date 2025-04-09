import { createToast, ToastType } from './createToast';
import { getStorageValue, setStorageValue } from './storage';
import { button, resetFieldId, select, toggle } from "./form";
import Cookies from 'js-cookie';
import { getCompilerLibraries } from "./compilerLibraries";

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
}
