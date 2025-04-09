import { getStorageValue } from './storage';

export function getLibrariesManifest(): void
{
    // @ts-ignore
    axios.get('/sanctum/csrf-cookie')
        .then(() =>
        {
            // @ts-ignore
            axios.get("/api/libraries")
                // @ts-ignore
                .then((response) =>
                {
                    // @ts-ignore
                    window.librariesManifest = response.data;
                })
                // @ts-ignore
                .catch((reasons) => console.error(reasons));
        })
        // @ts-ignore
        .catch((reasons) => console.error(reasons));    
}

export function getCompilerLibraries(): any
{
    return {
        olcPixelGameEngine: getStorageValue("olcPixelGameEngine"),
        miniaudio: getStorageValue("miniaudio"),
        olcPGEX_Gamepad: getStorageValue("olcPGEX_Gamepad"),
        olcPGEX_MiniAudio: getStorageValue("olcPGEX_MiniAudio"),
        olcSoundWaveEngine: getStorageValue("olcSoundWaveEngine")
    }
}
