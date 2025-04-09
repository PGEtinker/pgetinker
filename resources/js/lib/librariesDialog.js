import { createToast, ToastType } from './createToast';
import { getStorageValue, setStorageValue } from './storage';
import { button, resetFieldId, select, toggle } from "./form";
import Cookies from 'js-cookie';
import { getCompilerLibraries } from "./compilerLibraries";

export default function librariesDialog(state)
{
    return new Promise((resolve) =>
    {
        resetFieldId();

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
                
        dialog.querySelector(".ok").addEventListener("click", (event) =>
        {
            dialog.remove();
            resolve();
        });
                
        document.body.appendChild(dialog);
    });
}
