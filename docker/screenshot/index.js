import { configDotenv } from "dotenv";
import puppeteer from "puppeteer";
import express from "express";
import morgan from "morgan";
import Module from "module";

configDotenv();

const app = express();
const port = process.env.PORT || 6969;
const mode = process.env.MODE || 'production';

app.use(express.json({ limit: "20mb"}));
app.use(morgan("combined"));

const sleep = (delay) =>
{
    return new Promise((resolve) =>
    {
        console.log("Screenshot:", `sleep ${delay} seconds`);
        setTimeout(() =>
        {
            resolve();
        }, delay * 1000);
    });
}

app.post("/", async(request, response) =>
{
    if(!request.body.html)
    {
        response.statusCode = 400;
        response.send({
            statusCode: 400,
            message: "Missing required parameter",
        });
        return;
    }
    
    // open the browser
    const browser = await puppeteer.launch({
        executablePath: '/usr/bin/google-chrome',
        headless: true,
        args: [
            '--no-sandbox',
            '--disable-setuid-sandbox',
            '--enable-webgl',
            '--use-gl=desktop',
            '--enable-gpu',
            '--enable-unsafe-swiftshader',
            '--use-fake-ui-for-media-stream',
            '--use-fake-device-for-media-stream',
            '--autoplay-policy=user-gesture-required',
            '--mute-audio',
        ],
    });

    // open the tab
    const page = await browser.newPage();
    console.log("Screenshot:", "new page");

    // Capture console messages
    page.on('console', message =>
    {
        const type = message.type().toUpperCase();
        const text = message.text();
        console.log(`[PAGE CONSOLE ${type}] ${text}`);
    });
  
    // Capture page errors
    page.on('pageerror', error =>
    {
        console.error('[PAGE ERROR]', error.message);
    });    

    // set the page's content
    await page.setContent(request.body.html);
    console.log("Screenshot:", "set html content");

    try
    {
        // setup core update handler
        await page.evaluate(() =>
        {
            const pgeUpdateHandler = () =>
            {
                console.log("PGE: core update called.");
                Module.canvas.removeEventListener("pge-core-update", pgeUpdateHandler)
                Module.pgeReadyForScreenshot = true;
            }
            
            Module.pgeReadyForScreenshot = false;
            Module.canvas.addEventListener("pge-core-update", pgeUpdateHandler);
        });
        console.log("Screenshot:", "setup core update handler")

        // wait for pge to be ready
        await page.waitForFunction(() =>
        {
            return Module.pgeReadyForScreenshot;
        });
        console.log("Screenshot:", "pge is ready for screenshot");

        // wait for 5 second after ready
        await sleep(5);

        // get the PGE canvas
        const canvas = await page.$('canvas');
        console.log("Screenshot:", "get the canvas");

        // get the size of the PGE canvas
        const boundingBox = await canvas.boundingBox();
        console.log("Screenshot:", "get bounding box", boundingBox);
        
        // change the size of the viewport to match the PGE canvas size
        await page.setViewport({
            width: boundingBox.width,
            height: boundingBox.height
        });
        console.log("Screenshot:", "resize window to size of bounding box");
        
        // shutter --- click 
        const screenshot = await page.screenshot({
            type: "png",
            encoding: "binary",
        });
        console.log("Screenshot:", "take screenshot");
        
        // close the tab
        await page.close();
        console.log("Screenshot:", "close tab");
        
        response.statusCode = 200;
        response.header("Content-Type", "image/png");
        response.send(screenshot);
    }
    catch(e)
    {
        response.statusCode = 400;
        response.send(null);
    }

    // close the browser
    await browser.close();
    console.log("Screenshot:", "close browser");

});

app.listen(port, () =>
{
    console.log(`Screenshotter listening on port ${port}`);
});
