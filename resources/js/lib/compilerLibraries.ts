import { getStorageValue } from './storage';

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
