#pragma once
/*
	olcPGEX_MiniAudio.h

	+-------------------------------------------------------------+
	|         OneLoneCoder Pixel Game Engine Extension            |
	|                     MiniAudio v1.7                          |
	+-------------------------------------------------------------+

	NOTE: UNDER ACTIVE DEVELOPMENT - THERE MAY BE BUGS/GLITCHES
    
    What is this?
	~~~~~~~~~~~~~
    This extension abstracts the very robust and powerful miniaudio
    library. It provides simple loading and playback of WAV and MP3
    files. Because it's built on top of miniaudio, it requires next
    to no addictional build configurations in order to be built
    for cross-platform.

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2023 Moros Smith <moros1138@gmail.com>

	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:

	1. Redistributions or derivations of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce the above
	copyright notice. This list of conditions and the following	disclaimer must be
	reproduced in the documentation and/or other materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its contributors may
	be used to endorse or promote products derived from this software without specific
	prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS	"AS IS" AND ANY
	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
	OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
	SHALL THE COPYRIGHT	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
	INCIDENTAL,	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
	TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
	BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
	ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
	SUCH DAMAGE.

	Links
	~~~~~
	YouTube:	https://www.youtube.com/@Moros1138
	GitHub:		https://www.github.com/Moros1138
	Homepage:	https://www.moros1138.com
*/

#include "olcPixelGameEngine.h"
#include <exception>

#ifdef OLC_PGEX_MINIAUDIO
#define MINIAUDIO_IMPLEMENTATION
#endif

#include "miniaudio.h"

namespace olc
{
    class MiniAudio : public olc::PGEX
    {
    public:
        std::string name = "olcPGEX_MiniAudio v1.7";
    
    public:
        MiniAudio();
        ~MiniAudio();
        virtual bool OnBeforeUserUpdate(float& fElapsedTime) override;
        static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
        static bool backgroundPlay;

    public: // CONFIGURATION
        // set whether audio will continue playing when the app has lost focus
        void SetBackgroundPlay(bool state);

    public: // LOADING ROUTINES       
        const int LoadSound(const std::string& path);
        void UnloadSound(const int id);
    
    public: // PLAYBACK CONTROLS
        // plays a sample, can be set to loop
        void Play(const int id, const bool loop = false);
        // plays a sound file, as a one off, and automatically unloads it
        void Play(const std::string& path);
        // stops a sample, rewinds to beginning
        void Stop(const int id);
        // pauses a sample, does not change position
        void Pause(const int id);
        //  toggle between play and pause
        void Toggle(const int id, bool rewind = false);
    
    public: // SEEKING CONTROLS
        // seek to the provided position in the sound, by milliseconds
        void Seek(const int id, const unsigned long long milliseconds);
        // seek to the provided position in the sound, by float 0.f is beginning, 1.0f is end
        void Seek(const int id, const float& location);
        // seek forward from current position by the provided time
        void Forward(const int id, const unsigned long long milliseconds);
        // seek forward from current position by the provided time
        void Rewind(const int id, const unsigned long long milliseconds);

    public: // MISC CONTROLS
        // set volume of a sound, 0.0f is mute, 1.0f is full
        void SetVolume(const int id, const float& volume);
        // set pan of a sound, -1.0f is left, 1.0f is right, 0.0f is center
        void SetPan(const int id, const float& pan);
        // set pitch of a sound, 1.0f is normal
        void SetPitch(const int id, const float& pitch);
        
    public: // MISC INFORMATION
        // determine if a sound is playing
        bool IsPlaying(const int id);
        // gets the current position in the sound, in milliseconds
        unsigned long long GetCursorMilliseconds(const int id);
        // gets the current position in the sound, as a float between 0.0f and 1.0f
        float GetCursorFloat(const int id);


    public: // WAVEFORM AND NOISE GENERATION
        struct Waveform;
        // creates a new waveform and returns the id of the waveform
        const int CreateWaveform(const double amplitude, const double frequency, const ma_waveform_type waveformType);
        // starts playing a waveform, continues producing sound until stopped
        void PlayWaveform(const int id);
        // change the amplitude of a waveform (loudness)
        void SetWaveformAmplitude(const int id, const double amplitude);
        // change the frequency of a waveform (pitch)
        void SetWaveformFrequency(const int id, const double frequency);
        // change the type of a waveform
        void SetWaveformType(const int id, const ma_waveform_type waveformType);
        // stop a waveform from playing
        void StopWaveform(const int id);
        // unload and free resources of a given waveform
        void UnloadWaveform(const int id);

        // getters
        // whether or not a waveform is currently playing
        const bool IsWaveformPlaying(const int id) const;
        // returns waveform amplitude
        const double& GetWaveformAmplitude(const int id) const;
        // returns waveform frequency
        const double& GetWaveformFrequency(const int id) const;
        // returns waveform type
        const ma_waveform_type& GetWaveformType(const int id) const;
        // ADVANCED USAGE, retrieval of raw ma_waveform object
        ma_waveform* GetWaveform(const int id) const;

        // noise generation
        // set a noise callback function so your application can send sound updates.
        // the callback provides two floating point values to override, one for the left channel and one for the right channel. fill them with raw audio data.
        // for periodic functions, you can reference the fElapsedTime variable to track how much time passed this frame. Accumulate it somewhere to keep track of the total audio time.
        // if you do not change the output channels, the values previously used will be played.
        void SetNoiseCallback(std::function<void(float& noiseLeftChannel, float& noiseRightChannel, const float fElapsedTime)>callbackFunc);
        // clears the noise callback and resets the channel values to 0.0
        void ClearNoiseCallback();
        
    public: // ADVANCED FEATURES for those who want to use more of miniaudio
        // gets the currently loaded persistent sounds
        const std::vector<ma_sound*>& GetSounds() const;
        // gets the currently loaded one-off sounds
        const std::vector<ma_sound*>& GetOneOffSounds() const;
        // gets a pointer to the ma_device
        ma_device* GetDevice();
        // gets a pointer to the ma_engine
        ma_engine* GetEngine();

        struct Waveform{
            friend class MiniAudio;
            bool isPlaying{false};
            Waveform(const double amplitude, const double frequency, const ma_waveform_type waveformType, const ma_device&device);
        private:
            bool unloaded{false};
            ma_waveform waveform;
            ma_waveform_config waveformConfig;
        };

    private:        
        
        /*
            Soooo, i'm not going to spend a whole lot of time
            documenting miniaudio features, if you want to
            know more I invite you to visit their very very
            nicely documented webiste at:

            https://miniaud.io/docs/manual/index.html
        */
        
        ma_device device;
        ma_engine engine;
        ma_resource_manager resourceManager;
        
        // sample rate for the device and engine
        int sampleRate;
        // this is where the sounds are kept
        std::vector<ma_sound*> vecSounds;
        std::vector<ma_sound*> vecOneOffSounds;

        static std::vector<Waveform> vecWaveforms;

        static float noiseLeftChannel;
        static float noiseRightChannel;
        static std::function<void(float& out_data_channel_left, float& out_data_channel_right, const float fElapsedTime)> noiseCallback;
    };

    /**
     * EXCEPTIONS, long story short. I needed to be able
     * to construct the PGEX in a state where it could
     * fail at runtime. If you have a better way of
     * accomplishing the PGEX pattern without using
     * exceptions, I'm open to suggestions!
    */
    struct MiniAudioDeviceException : public std::exception
    {
        const char* what() const throw()
        {
            return "Failed to initialize a device.";
        }
    };
    
    struct MiniAudioResourceManagerException : public std::exception
    {
        const char* what() const throw()
        {
            return "Failed to initialize the resource manager.";
        }
    };

    struct MiniAudioEngineException : public std::exception
    {
        const char* what() const throw()
        {
            return "Failed to initialize the audio engine.";
        }
    };

    struct MiniAudioSoundException : public std::exception
    {
        const char* what() const throw()
        {
            return "Failed to initialize a sound.";
        }
    };

    struct MiniAudioWaveformException : public std::exception
    {
        const char* what() const throw()
        {
            return "Failed to initialize a waveform.";
        }
    };
}


#ifdef OLC_PGEX_MINIAUDIO
#undef OLC_PGEX_MINIAUDIO

namespace olc
{
    bool MiniAudio::backgroundPlay = false;
    std::vector<MiniAudio::Waveform> MiniAudio::vecWaveforms;
    float MiniAudio::noiseLeftChannel{};
    float MiniAudio::noiseRightChannel{};
    std::function<void(float& out_audio_data_left, float& out_audio_data_right, const float fElapsedTime)> MiniAudio::noiseCallback;
    
    MiniAudio::MiniAudio() : olc::PGEX(true)
    {
        sampleRate = 48000;
        
        ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
        deviceConfig.playback.format = ma_format_f32;
        deviceConfig.playback.channels = 2;
        deviceConfig.sampleRate = sampleRate;
        deviceConfig.dataCallback = MiniAudio::data_callback;
        deviceConfig.pUserData = &engine;

        if(ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS)
            throw MiniAudioDeviceException();

        ma_resource_manager_config resourceManagerConfig = ma_resource_manager_config_init();
        resourceManagerConfig.decodedFormat     = ma_format_f32;
        resourceManagerConfig.decodedChannels   = 0;
        resourceManagerConfig.decodedSampleRate = sampleRate;
        
        #ifdef __EMSCRIPTEN__
            resourceManagerConfig.jobThreadCount = 0;                           
            resourceManagerConfig.flags |= MA_RESOURCE_MANAGER_FLAG_NON_BLOCKING;
            resourceManagerConfig.flags |= MA_RESOURCE_MANAGER_FLAG_NO_THREADING;
        #endif

        if(ma_resource_manager_init(&resourceManagerConfig, &resourceManager) != MA_SUCCESS)
            throw MiniAudioResourceManagerException();
        
        ma_engine_config engineConfig = ma_engine_config_init();
        engineConfig.pDevice = &device;
        engineConfig.pResourceManager = &resourceManager; 
        
        if(ma_engine_init(&engineConfig, &engine) != MA_SUCCESS)
            throw MiniAudioEngineException();
        
        MiniAudio::backgroundPlay = false; 
    }

    MiniAudio::~MiniAudio()
    {
        for(auto sound : vecSounds)
        {
            if(sound != nullptr)
            {
                ma_sound_uninit(sound);
                delete sound;
            }
        }
            
        ma_resource_manager_uninit(&resourceManager);

        ma_engine_uninit(&engine);        
    }

    bool MiniAudio::OnBeforeUserUpdate(float& fElapsedTime)
    {
        #ifdef __EMSCRIPTEN__
        ma_resource_manager_process_next_job(&resourceManager);
        #endif

        for(int i = 0; i < vecOneOffSounds.size(); i++)
        {
            if(!ma_sound_is_playing(vecOneOffSounds.at(i)))
            {
                ma_sound_uninit(vecOneOffSounds.at(i));
                vecOneOffSounds.erase(vecOneOffSounds.begin() + i);
                break;
            }
        }

        return false;
    }

    void MiniAudio::data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
    {
        if(!MiniAudio::backgroundPlay && !pge->IsFocused())
            return;

        /*
            The way mixing works is that we just read into a temporary buffer, then take the contents of that buffer and mix it with the
            contents of the output buffer by simply adding the samples together. You could also clip the samples to -1..+1, but I'm not
            doing that in this example.
        */

        const int CHANNEL_COUNT{2};
        using PlaybackFormat = float;

        ma_result result;
        float temp[4096];
        ma_uint32 tempCapInFrames = ma_countof(temp) / CHANNEL_COUNT;
        ma_uint32 totalFramesRead = 0;
        
        while (totalFramesRead < frameCount) {
            ma_uint64 iSample;
            ma_uint64 framesReadThisIteration{};
            ma_uint32 totalFramesRemaining = frameCount - totalFramesRead;
            ma_uint32 framesToReadThisIteration = tempCapInFrames;
            if (framesToReadThisIteration > totalFramesRemaining) {
                framesToReadThisIteration = totalFramesRemaining;
            }

            if(noiseCallback)
            {
                for(int i = 0; i < frameCount; i++)
                {
                    // we send multiple callbacks into the future to get what sound we should be playing...
                    // for raw music data from programs like emulators, the user will probably just keep sending the same sound until it changes on their end.
                    // for periodic functions, they can use the elapsed time this callback sends to accurately determine what
                    // data should be sent precisely at that moment...
                    noiseCallback(noiseLeftChannel, noiseRightChannel, 1.f / pDevice->sampleRate);

                    // Since we're reading each channel in individually, They have to be interlaced. Each frame we read one value from the left and right channel...
                    // From the miniaudio documentation
                    // ********************************
                    // A "frame" is one sample for each channel. For example, in a stereo stream (2 channels), one frame is 2 samples: one for the left, one for the right.
                    // ********************************
                    // add to current output. NOTE: we are assuming a channel count of 2!!!
                    // NOTE: we are assuming a floating point playback format!!!
                    ((PlaybackFormat*)pOutput)[totalFramesRead + i*CHANNEL_COUNT] += noiseLeftChannel;
                    ((PlaybackFormat*)pOutput)[totalFramesRead + i*CHANNEL_COUNT + 1] += noiseRightChannel;
                }
            }

            // read audio data from basic ma engine
            result = ma_engine_read_pcm_frames((ma_engine*)(pDevice->pUserData), temp, frameCount, &framesReadThisIteration);

            if (result == MA_SUCCESS && framesReadThisIteration > 0)
            {
                // add to current output. NOTE: we are assuming a channel count of 2!!!
                for (iSample = 0; iSample < framesReadThisIteration*CHANNEL_COUNT; ++iSample) {
                    // NOTE: we are assuming a floating point playback format!!!
                    ((PlaybackFormat*)pOutput)[totalFramesRead*CHANNEL_COUNT + iSample] += temp[iSample];
                }
            }

            for(Waveform&waveform : MiniAudio::vecWaveforms)
            {
                if(waveform.unloaded)
                    continue;

                if(waveform.isPlaying)
                {
                    // read audio data from a waveform
                    result = ma_waveform_read_pcm_frames(&waveform.waveform, temp, frameCount, &framesReadThisIteration);
                    if (result != MA_SUCCESS || framesReadThisIteration == 0) {
                        continue;
                    }
                    else
                    {
                        // splice it together with other audio data
                        for (iSample = 0; iSample < framesReadThisIteration*CHANNEL_COUNT; ++iSample) {
                            ((float*)pOutput)[totalFramesRead*CHANNEL_COUNT + iSample] += temp[iSample];
                        }
                    }
                }
            }

            totalFramesRead += (ma_uint32)framesReadThisIteration;

            if (framesReadThisIteration < (ma_uint32)framesToReadThisIteration) {
                break;  /* Reached EOF. */
            }
        }
    }
    
    void MiniAudio::SetBackgroundPlay(bool state)
    {
        MiniAudio::backgroundPlay = state;
    }

    const int MiniAudio::LoadSound(const std::string& path)
    {
        // create the sound
        ma_sound* sound = new ma_sound();

        // load it from the file and decode it
        if(ma_sound_init_from_file(&engine, path.c_str(), MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC, NULL, NULL, sound) != MA_SUCCESS)
            throw MiniAudioSoundException();
        
        // attempt to re-use an empty slot
        for(int i = 0; i < vecSounds.size(); i++)
        {
            if(vecSounds.at(i) == nullptr)
            {
                vecSounds.at(i) = sound;
                return i;
            }
        }
        
        // no empty slots, make more room!
        const int id = vecSounds.size();
        vecSounds.push_back(sound);
        
        return id;
    }
    
    void MiniAudio::UnloadSound(const int id)
    {
        ma_sound_uninit(vecSounds.at(id));
        delete vecSounds.at(id);
        vecSounds.at(id) = nullptr;
    }

    void MiniAudio::Play(const int id, const bool loop)
    {
        if(ma_sound_is_playing(vecSounds.at(id)))
        {
            ma_sound_seek_to_pcm_frame(vecSounds.at(id), 0);
            return;
        }
        
        ma_sound_set_looping(vecSounds.at(id), loop);
        ma_sound_start(vecSounds.at(id));
    }

    void MiniAudio::Play(const std::string& path)
    {
        // create the sound
        ma_sound* sound = new ma_sound();

        // load it from the file and decode it
        if(ma_sound_init_from_file(&engine, path.c_str(), MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC, NULL, NULL, sound) != MA_SUCCESS)
            throw MiniAudioSoundException();
        
        ma_sound_start(sound);
        vecOneOffSounds.push_back(sound);
    }

    void MiniAudio::Stop(const int id)
    {
        ma_sound_seek_to_pcm_frame(vecSounds.at(id), 0);
        ma_sound_stop(vecSounds.at(id));
    }

    void MiniAudio::Pause(const int id)
    {
        auto it = vecSounds.begin() + id;
        ma_sound_stop(vecSounds.at(id));
    }

    void MiniAudio::Toggle(const int id, bool rewind)
    {
        if(ma_sound_is_playing(vecSounds.at(id)))
        {
            ma_sound_stop(vecSounds.at(id));

            if(rewind)
                ma_sound_seek_to_pcm_frame(vecSounds.at(id), 0);
            
            return;
        }
        
        ma_sound_start(vecSounds.at(id));
    }
    
    void MiniAudio::Seek(const int id, const unsigned long long milliseconds)
    {
        unsigned long long frame = (milliseconds * engine.sampleRate) / 1000;
        
        ma_sound_seek_to_pcm_frame(vecSounds.at(id), frame);
    }

    void MiniAudio::Seek(const int id, const float& location)
    {
        unsigned long long length;
        ma_sound_get_length_in_pcm_frames(vecSounds.at(id), &length);

        unsigned long long frame = length * location;
        
        ma_sound_seek_to_pcm_frame(vecSounds.at(id), frame);
    }

    void MiniAudio::Forward(const int id, const unsigned long long milliseconds)
    {
        unsigned long long cursor;
        ma_sound_get_cursor_in_pcm_frames(vecSounds.at(id), &cursor);

        unsigned long long frame = (milliseconds * engine.sampleRate) / 1000;
        ma_sound_seek_to_pcm_frame(vecSounds.at(id), cursor + frame);
    }

    void MiniAudio::Rewind(const int id, const unsigned long long milliseconds)
    {
        unsigned long long cursor;
        ma_sound_get_cursor_in_pcm_frames(vecSounds.at(id), &cursor);

        unsigned long long frame = (milliseconds * engine.sampleRate) / 1000;
        ma_sound_seek_to_pcm_frame(vecSounds.at(id), cursor - frame);
    }

    void MiniAudio::SetVolume(const int id, const float& volume)
    {
        ma_sound_set_volume(vecSounds.at(id), volume);
    }

    void MiniAudio::SetPan(const int id, const float& pan)
    {
        ma_sound_set_pan(vecSounds.at(id), pan);
    }
    
    void MiniAudio::SetPitch(const int id, const float& pitch)
    {
        ma_sound_set_pitch(vecSounds.at(id), pitch);
    }

    unsigned long long MiniAudio::GetCursorMilliseconds(const int id)
    {
        unsigned long long cursor;
        ma_sound_get_cursor_in_pcm_frames(vecSounds.at(id), &cursor);
        
        cursor *= 1000;
        cursor /= sampleRate;
        
        return cursor;
    }

    bool MiniAudio::IsPlaying(const int id)
    {
        return ma_sound_is_playing(vecSounds.at(id));
    }

    float MiniAudio::GetCursorFloat(const int id)
    {
        unsigned long long cursor;
        ma_sound_get_cursor_in_pcm_frames(vecSounds.at(id), &cursor);

        unsigned long long length;
        ma_sound_get_length_in_pcm_frames(vecSounds.at(id), &length);

        return (float)cursor / length;
    }

    const std::vector<ma_sound*>& MiniAudio::GetSounds() const
    {
        return vecSounds;
    }

    const std::vector<ma_sound*>& MiniAudio::GetOneOffSounds() const
    {
        return vecOneOffSounds;
    }

    ma_device* MiniAudio::GetDevice()
    {
        return &device;
    }

    ma_engine* MiniAudio::GetEngine()
    {
        return &engine;
    }

    const int MiniAudio::CreateWaveform(const double amplitude, const double frequency, const ma_waveform_type waveformType)
    {
        // attempt to re-use an empty slot
        for(int i = 0; i < vecWaveforms.size(); i++)
        {
            if(vecWaveforms.at(i).unloaded)
            {
                vecWaveforms.at(i) = Waveform{amplitude, frequency, waveformType, device};
                return i;
            }
        }

        // no empty slots, make more room!
        const int id = vecWaveforms.size();
        vecWaveforms.emplace_back(amplitude, frequency, waveformType, device);

        return id;
    }

    void MiniAudio::PlayWaveform(const int id)
    {
        vecWaveforms.at(id).isPlaying = true;
    }

    void MiniAudio::SetWaveformAmplitude(const int id, const double amplitude)
    {
        ma_waveform_set_amplitude(&vecWaveforms.at(id).waveform, amplitude);
    }

    void MiniAudio::SetWaveformFrequency(const int id, const double frequency)
    {
        ma_waveform_set_frequency(&vecWaveforms.at(id).waveform, frequency);
    }

    void MiniAudio::SetWaveformType(const int id, const ma_waveform_type waveformType)
    {
        ma_waveform_set_type(&vecWaveforms.at(id).waveform, waveformType);
    }

    void MiniAudio::StopWaveform(const int id)
    {
        vecWaveforms.at(id).isPlaying = false;
    }

    void MiniAudio::UnloadWaveform(const int id)
    {
        ma_waveform_uninit(&vecWaveforms.at(id).waveform);
        vecWaveforms.at(id).unloaded = true;
    }

    ma_waveform* MiniAudio::GetWaveform(const int id) const
    {
        return &vecWaveforms.at(id).waveform;
    }

    const bool MiniAudio::IsWaveformPlaying(const int id) const
    {
        return vecWaveforms.at(id).isPlaying;
    }

    const double& MiniAudio::GetWaveformAmplitude(const int id) const
    {
        return vecWaveforms.at(id).waveform.config.amplitude;
    }

    const double& MiniAudio::GetWaveformFrequency(const int id) const
    {
        return vecWaveforms.at(id).waveform.config.frequency;
    }

    const ma_waveform_type&MiniAudio::GetWaveformType(const int id) const
    {
        return vecWaveforms.at(id).waveform.config.type;
    }

    MiniAudio::Waveform::Waveform(const double amplitude, const double frequency, const ma_waveform_type waveformType, const ma_device&device)
    {
        waveformConfig = ma_waveform_config_init(device.playback.format, device.playback.channels, device.sampleRate, waveformType, amplitude, frequency);
        if(ma_waveform_init(&waveformConfig, &waveform) != MA_SUCCESS)
            throw MiniAudioWaveformException();
    }

    void MiniAudio::SetNoiseCallback(std::function<void(float& noiseLeftChannel, float& noiseRightChannel, const float fElapsedTime)>callbackFunc)
    {
        noiseCallback = callbackFunc;
    }

    void MiniAudio::ClearNoiseCallback()
    {
        MiniAudio::noiseLeftChannel = 0.f;
        MiniAudio::noiseRightChannel = 0.f;
        noiseCallback = {};
    }

} // olc

#endif
