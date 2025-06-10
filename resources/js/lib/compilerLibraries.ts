import axios from 'axios';
import { getStorageValue } from './storage';

interface LibraryVersions {
  [key: string]: string[];
}

interface LibrariesManifest {
  latest: { [key: string]: string };
  libraries: LibraryVersions;
  statusCode: number;
}

interface Window {
    librariesManifest: LibrariesManifest;
    AppVersion: string;
}

declare const window: Window;

export async function getLibrariesManifest(): Promise<void>
{
    await axios.get('/sanctum/csrf-cookie');
    const manifest: LibrariesManifest = (await axios.get("/api/libraries")).data;
    if(!(window.AppVersion.indexOf("develop") !== -1))
    {
        window.librariesManifest = {
            ...manifest,
            libraries: Object.fromEntries(
                Object.entries(manifest.libraries).map(([key, versions]) => [
                    key,
                    versions.filter(version => version !== "dev")
                ])
            )
        };
    }
    else
    {
        window.librariesManifest = manifest;
    }
}

export function getCompilerLibraries(): any
{
    return {
        entt: getStorageValue("entt"),
        miniaudio: getStorageValue("miniaudio"),
        olcPGEX_Gamepad: getStorageValue("olcPGEX_Gamepad"),
        olcPGEX_MiniAudio: getStorageValue("olcPGEX_MiniAudio"),
        olcPixelGameEngine: getStorageValue("olcPixelGameEngine"),
        olcSoundWaveEngine: getStorageValue("olcSoundWaveEngine"),
        raylib: getStorageValue("raylib"),
    }
}
