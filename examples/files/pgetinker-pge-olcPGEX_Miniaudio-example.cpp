/*******************************************************************************************
*
*   olcPixelGameEngine example - olcPGEX_MiniAudio Demo
*   
********************************************************************************************
*   
*   This is free and unencumbered software released into the public domain.
*   
*   Anyone is free to copy, modify, publish, use, compile, sell, or
*   distribute this software, either in source code form or as a compiled
*   binary, for any purpose, commercial or non-commercial, and by any
*   means.
*   
*   In jurisdictions that recognize copyright laws, the author or authors
*   of this software dedicate any and all copyright interest in the
*   software to the public domain. We make this dedication for the benefit
*   of the public at large and to the detriment of our heirs and
*   successors. We intend this dedication to be an overt act of
*   relinquishment in perpetuity of all present and future rights to this
*   software under copyright law.
*   
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
*   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
*   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
*   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
*   OTHER DEALINGS IN THE SOFTWARE.
*   
*   For more information, please refer to <https://unlicense.org>
*
********************************************************************************************/
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_MINIAUDIO
#include "olcPGEX_MiniAudio.h"

#include "pgetinker.h"

constexpr float thirtyFramesPerSecond = 1.0f / 30.f;

class Demo : public olc::PixelGameEngine
{
public:
    Demo()
    {
        sAppName = "Demo MiniAudio";
    }
    
public:
    bool OnUserCreate() override
    {
        pgetinker_file_resolve("https://raw.githubusercontent.com/PGEtinker/assets/main/song1.mp3", "assets/sounds/song1.mp3");
        pgetinker_file_resolve("https://raw.githubusercontent.com/PGEtinker/assets/main/SampleA.wav", "assets/sounds/SampleA.wav");

        /**
         * load a sample from a file. currently decodes WAV and MP3
         * files without further coding or configuration.
         * 
         * returns a sample ID (int), for control and expression calls.
         */
        song1 = ma.LoadSound("assets/sounds/song1.mp3");

        /**
         * this is here to demonstrate how the adventurous can
         * exploit other features of miniaudio that hasn't been
         * abstracted by the PGEX
         * 
         * Here you get a pointer to a sample's ma_sound.
         */
        ma_sound_set_position(ma.GetSound(song1), 0.0f, 0.0f, 0.0f);
        return true;
    }
    
    bool OnUserUpdate(float fElapsedTime) override
    {
        fElapsedTime = (fElapsedTime > thirtyFramesPerSecond) ? thirtyFramesPerSecond : fElapsedTime;

        if(GetKey(olc::Key::K1).bPressed)
        {
            backgroundPlay = !backgroundPlay;
            ma.SetBackgroundPlay(backgroundPlay);
        }

        if(GetKey(olc::Key::S).bPressed)
        {
            ma.Play("assets/sounds/SampleA.wav");
        }

        if(GetKey(olc::Key::SPACE).bPressed)
        {
            /**
             * Toggle takes a sample ID (int) and either
             * starts playback or pauses playback depending
             * on whether the sample is currently playing or not.
             */
            ma.Toggle(song1);
        }
            
        if(GetKey(olc::Key::MINUS).bHeld)
            pan -= 1.0f * fElapsedTime;
            
        if(GetKey(olc::Key::EQUALS).bHeld)
            pan += 1.0f * fElapsedTime;

        if(GetKey(olc::Key::OEM_4).bHeld)
            pitch -= 1.0f * fElapsedTime;

        if(GetKey(olc::Key::OEM_6).bHeld)
            pitch += 1.0f * fElapsedTime;

        if(GetKey(olc::Key::DOWN).bHeld)
            volume -= 1.0f * fElapsedTime;
            
        if(GetKey(olc::Key::UP).bHeld)
            volume += 1.0f * fElapsedTime;
        
        if(GetKey(olc::Key::LEFT).bHeld)
            distance -= 10.0f * fElapsedTime;
            
        if(GetKey(olc::Key::RIGHT).bHeld)
            distance += 10.0f * fElapsedTime;

        // Reset pan, pitch, and volume
        if(GetKey(olc::Key::R).bPressed)
        {
            pan = 0.0f;
            pitch = 1.0f;
            volume = 1.0f;
            distance = 0.0f;
        }
        
        /**
         * this is here to demosntrate how the adventurous can exploit other
         * features of miniaudio that haven't been abstracted by the PGEX.
         */
        distance = std::clamp(distance, 0.0f, 100.0f);
        ma_engine_listener_set_position(ma.GetEngine(), 0, 0.0f, distance, 0.0f);

        
        /**
         * SetPan takes a sample ID (int) and a float
         * -1.0f to 1.0f where 0 is center.
         */
        pan = std::clamp(pan, -1.0f, 1.0f);
        ma.SetPan(song1, pan);

        /**
         * SetPitch takes a sample ID (int) and a float
         * 1.0f is normal pitch.
         */
        pitch  = std::clamp(pitch,  0.0f, 2.0f);
        ma.SetPitch(song1, pitch);

        /**
         * SetVolume takes a sample ID (int) and a float
         * 0.0f to 1.0f where 1.0f is full volume.
         */
        volume = std::clamp(volume, 0.0f, 1.0f);
        ma.SetVolume(song1, volume);
        
        /**
         * GetCursorFloat takes a sample ID (int) and returns
         * a float 0.0f to 1.0f, nearer to 1.0f is nearer the end.
         */
        seek = ma.GetCursorFloat(song1);

        /**
         * GetCursorMilliseconds takes a sample ID (int) and returns
         * the current playback position, in milliseconds.
         */
        cursor = ma.GetCursorMilliseconds(song1);

        // Draw Instructions and Indicators
        Clear(olc::BLACK);

        if(ma.IsPlaying(song1))
            Clear(olc::VERY_DARK_BLUE);

        DrawStringDecal({5, 5}, \
            "-------- INFO --- CONTROLS -\n"
            "\n"
            "Pan    <" + std::to_string(pan)    + ">   -, =\n"
            "\n"
            "Pitch  <" + std::to_string(pitch)  + ">   [, ]\n"
            "\n"
            "Volume <" + std::to_string(volume) + "> Up, Down\n"
            "\n"
            "Distance <" + std::to_string(distance) + "> Left, Right\n"
            "\n"
            "One-Off Sounds <" + std::to_string(ma.GetOneOffCount()) + ">   S\n" \
            "\n" \
            "BackgroundPlay <" + ((backgroundPlay) ? "On": "Off") + "> K1\n",
        olc::WHITE, {0.5f, 0.5f});

        olc::vi2d center = (GetScreenSize() / 2);
        constexpr float scale = 1.2f;
        DrawStringDecal((center - olc::vi2d{0, 24}) - (olc::vf2d(GetTextSize(ma.name + " Demo")) * scale / 2.0f), ma.name + " Demo", olc::WHITE, {scale, scale});
        DrawStringDecal(center - (GetTextSize("Hit <SPACE> To Toggle Playback") / 2), "Hit <SPACE> To Toggle Playback", olc::WHITE);
        DrawStringDecal((center + olc::vi2d{0, 16}) - (GetTextSize("Hit <R> TO Reset Pan/Pitch/Volume") / 2), "Hit <R> TO Reset Pan/Pitch/Volume", olc::WHITE);
        
        DrawStringDecal({5, 144}, \
            "Cursor (ms): " + std::to_string(cursor),
        olc::WHITE, {0.5f, 0.5f});

        DrawStringDecal({5, 160}, \
            "Music: Joy Ride [Full version] by MusicLFiles\n"
            "Free download: https://filmmusic.io/song/11627-joy-ride-full-version\n"
            "Licensed under CC BY 4.0: https://filmmusic.io/standard-license\n", \
        olc::WHITE, {0.5f, 0.5f});
        
        // Draw The Playback Cursor (aka the position in the sound file)
        FillRect({0, 175}, { (int)(ScreenWidth() * seek), 10 }, olc::YELLOW);
        
        #if defined(__EMSCRIPTEN__)
            return true;
        #else
            return !GetKey(olc::Key::ESCAPE).bPressed;
        #endif
    }

    // The instance of the audio engine, no fancy config required.
    olc::MiniAudio ma;
    
    // To keep track of our sample ID
    int song1;
    
    // For demonstration controls, with sensible default values
    float pan    = 0.0f;
    float pitch  = 1.0f;
    float seek   = 0.0f;
    float volume = 1.0f;
    float distance = 0.0f;
    bool backgroundPlay = false;
    ma_uint64 cursor = 0ull;

};

int main()
{
    Demo demo;
    if (demo.Construct(320, 180, 4, 4))
        demo.Start();
    return 0;
}