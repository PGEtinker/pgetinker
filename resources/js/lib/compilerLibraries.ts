import axios from 'axios';
import { getStorageValue } from './storage';

export async function getLibrariesManifest(): Promise<void>
{
    // @ts-ignore
    await axios.get('/sanctum/csrf-cookie');
    // @ts-ignore
    window.librariesManifest = (await axios.get("/api/libraries")).data;
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
