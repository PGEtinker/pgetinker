export default function developDialog()
{
    return new Promise((resolve, reject) =>
    {
        let dialog = document.createElement("div");
    
        dialog.classList.toggle("dialog", true);
        dialog.classList.toggle("develop-branch", true);

        dialog.innerHTML = `
        <div class="window">
            <div class="header">Develop Branch Deployment!</div>
            <div class="content">
                <p>
                    <img src="/images/stop.png" alt="picture of a stop sign">
                </p>
                <p>
                    This version of PGEtinker is a development deployment
                    and should not be considered stable. At any moment
                    it could be brought offline and it's data purged.
                </p>
                <h3>What should I do?</h3>
                <p>
                    If you're developing or testing PGEtinker then it's
                    safe to ignore this message and carry on with your work.
                    However, if you got here expecting to use the PGEtinker
                    platform then you should head over to the main PGEtinker
                    site.
                </p>
                <hr>
                <p>
                    <label for="disable-develop-branch-warning">
                        <input type="checkbox" id="disable-develop-branch-warning" name="disable-develop-branch-warning">
                        Don't show me this warning again.
                    </label>
                </p>
            </div>
            <div class="footer">
                <button type="button" class="back">Proceed</button>
                <button type="button" class="ok">Go to PGEtinker.com</button>
            </div>
        </div>`;
        
        dialog.querySelector(".ok").addEventListener("click", (event) =>
        {
            reject();
            dialog.remove();
        });

        dialog.querySelector(".back").addEventListener("click", (event) =>
        {
            resolve(dialog.querySelector("#disable-develop-branch-warning").checked);
            dialog.remove();
        });
        
        document.body.appendChild(dialog);
    });

}