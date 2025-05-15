export default function examplesDialog()
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
                <iframe src="/examples?framed=true"></iframe>
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
                
        document.body.appendChild(dialog);
    });

}