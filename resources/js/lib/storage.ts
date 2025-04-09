import { getLibrariesManifest, getCompilerLibraries } from "./compilerLibraries";
// @ts-ignore
import Cookies from 'js-cookie';

interface Window {
    librariesManifest: any;
    localStorage: Storage;
}

declare const window: Window;
const prefix: string = "pgetinker_";
const store: Storage = window.localStorage;

export async function conformStorage(): Promise<void>
{
    await getLibrariesManifest();

    let agree = store.getItem("pgetinkerAgreedToTerms");
    store.removeItem("pgetinkerAgreedToTerms");
    if(agree)
    {
        setStorageValue("agreed-to-terms", true);
    }
    
    let code = store.getItem("pgetinkerCode");
    store.removeItem("pgetinkerCode");
    if(code)
    {
        setStorageValue("code", JSON.parse(code));
    }

    if(getStorageValue("theme") == null)
    {
        let theme = store.getItem("pgetinkerTheme");
        store.removeItem("pgetinkerTheme");
        if(theme)
        {
            setStorageValue("theme", theme);
        }
        else
        {
            setStorageValue("theme", "dark");
        }
    }

    if(getStorageValue("diagnostics.javidMode") == null)
    {
        setStorageValue("diagnostics.javidMode", false);
    }
    
    if(getStorageValue("editor.inlayHints.enabled") == null)
    {
        setStorageValue("editor.inlayHints.enabled", false);
    }
    
    if(getStorageValue("olcPixelGameEngine") == null)
    {
        setStorageValue("olcPixelGameEngine", "v2.25");
    }

    if(getStorageValue("miniaudio") == null)
    {
        setStorageValue("miniaudio", "0.11.21");
    }
    
    if(getStorageValue("olcPGEX_Gamepad") == null)
    {
        setStorageValue("olcPGEX_Gamepad", "f5958a5");
    }
    
    if(getStorageValue("olcPGEX_MiniAudio") == null)
    {
        setStorageValue("olcPGEX_MiniAudio", "v1.5");
    }

    if(getStorageValue("olcSoundWaveEngine") == null)
    {
        setStorageValue("olcSoundWaveEngine", "v0.02");
    }

    Cookies.set("pgetinker_libraries", encodeURIComponent(JSON.stringify(getCompilerLibraries())));
}

export function getStorageValue(key: string): string | null
{
    let value: string | null = store.getItem(prefix + key);

    if(typeof value !== "string")
    {
        return null;
    }
    
    try
    {
        return JSON.parse(value);
    }
    catch(e)
    {
        // it's just a string, return it
    }

    return value;
}

export function setStorageValue(key: string, value: string | any): void
{
    if(typeof value === "string")
    {
        store.setItem(prefix + key, value);
        return;
    }

    try
    {
        store.setItem(prefix + key, JSON.stringify(value));
    }
    catch(error)
    {
        console.error("setStorageValue failed to JSON.stringify value", error);
    }
}

export function removeStorageKey(key: string): void
{
    store.removeItem(prefix + key);
}

