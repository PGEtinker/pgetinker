import version from "./version";

export default function newsDialog()
{
    return new Promise((resolve) =>
    {
        let dialog = document.createElement("div");
    
        dialog.classList.toggle("dialog", true);
        dialog.classList.toggle("news", true);
        
        dialog.innerHTML = `
        <div class="window">
            <div class="header">News and Updates</div>
            <div class="content">
                <h3>PGEtinker has been updated. Here's what's been done:</h3>
                <iframe src="/release-notes?framed=true"></iframe>
                <p>
                    For more details visit the <a href="https://github.com/Moros1138/PGEtinker" target="_blank">PGEtinker github repository</a>.
                </p>
                <p>
                Version: ${version}
                </p>
            </div>
            <div class="footer">
                <button class="ok">Close</button>
            </div>
        </div>`;
    
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