#pragma once
/*
	olcPGEX_MiniAudio.h

	+-------------------------------------------------------------+
	|         OneLoneCoder Pixel Game Engine Extension            |
	|                     MiniAudio v2.0                          |
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

#ifdef OLC_PGEX_MINIAUDIO
#define MINIAUDIO_IMPLEMENTATION
#endif

#include "miniaudio.h"

#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <source_location>
#include <string>
#include <unordered_map>
#include <vector>

#undef DEVICE_TYPE //Stupid Windows

namespace olc
{
    
    class MiniAudio : public olc::PGEX
    {
    public:
        std::string name = "olcPGEX_MiniAudio v2.0";
    
    public: // structs
        struct SoundFileBuffer
        {
            SoundFileBuffer();
            SoundFileBuffer(ma_engine* engine);
            ~SoundFileBuffer();

            bool Load(const std::string& path, olc::ResourcePack* pack = nullptr);
            void Unload();

            std::vector<char> m_buffer;
            int m_count = 0;
            ma_engine* m_engine;
            bool m_loaded = false;
            std::string m_path;
        };
        
        struct Sound
        {
            ma_sound m_sound;
            ma_uint64 m_length_in_pcm_frames = 0;
            float m_length_in_seconds = 0;
            std::string m_path;
            bool m_play_once = false;
            
            const std::string string();
        };

        class Waveform
        {
        public:
            Waveform();
            Waveform(const double amplitude, const double frequency, const ma_waveform_type waveformType);
            
            void Start();
            void Stop();
            void Unload();

            bool IsPlaying() const;
            bool IsLoaded();
        
            ma_waveform* Get();

        private:
            bool m_is_playing = false;
            bool m_is_loaded  = false;
            
            ma_waveform m_waveform;
            ma_waveform_config m_waveform_config;
        };

    public:
        MiniAudio();
        ~MiniAudio();
        virtual bool OnBeforeUserUpdate(float& fElapsedTime) override;
        static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
        static std::vector<MiniAudio::Waveform> m_waveforms;

    public: // static variables
        static bool m_background_playback;
        static std::vector<float> m_engine_buffer;
        static std::vector<float> m_waveform_buffer;
        static std::function<void(float& out_data_channel_left, float& out_data_channel_right, const float fElapsedTime)> m_noise_callback;
        static float m_noise_left_channel;
        static float m_noise_right_channel;

    public: // configuration
        void SetBackgroundPlay(const bool state);

    public: // loading routines
        const int LoadSound(const std::string& path, olc::ResourcePack* pack = nullptr, bool playOnce = false);
        void UnloadSound(const int id);
    
    public: // playback routines
        // plays a sample, can be set to loop
        void Play(const int id, const bool looping = false);
        // plays a sound file, as a one off, and automatically unloads it
        const int Play(const std::string& path, olc::ResourcePack* pack = nullptr);
        // stops a sample, rewinds to beginning
        void Stop(const int id);
        // pauses a sample, does not change position
        void Pause(const int id);
        // toggle between play and pause
        void Toggle(const int id);

    public: // seeking controls
        // seek to the provided position in the sound, by milliseconds
        void Seek(const int id, const ma_uint64 milliseconds);
        // seek to the provided position in the sound, by float 0.f is beginning, 1.0f is end
        void Seek(const int id, const float& location);
        // seek forward from current position by the provided time
        void Forward(const int id, const ma_uint64 milliseconds);
        // seek forward from current position by the provided time
        void Rewind(const int id, const ma_uint64 milliseconds);

    public: // expression controls
        // set volume of a sound, 0.0f is mute, 1.0f is full
        void SetVolume(const int id, const float& volume);
        // set pan of a sound, -1.0f is left, 1.0f is right, 0.0f is center
        void SetPan(const int id, const float& pan);
        // set pitch of a sound, 1.0f is normal
        void SetPitch(const int id, const float& pitch);
    
    public: // misc information
        // determine if a sound is playing
        bool IsPlaying(const int id);
        // gets the current position in the sound, in milliseconds
        ma_uint64 GetCursorMilliseconds(const int id);
        // gets the current position in the sound, as a float between 0.0f and 1.0f
        float GetCursorFloat(const int id);
        // get the current number of active "one off" sounds 
        int GetOneOffCount();

    public: // waveforms
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

        // whether or not a waveform is currently playing
        const bool IsWaveformPlaying(const int id);
        // returns waveform amplitude
        const double& GetWaveformAmplitude(const int id);
        // returns waveform frequency
        const double& GetWaveformFrequency(const int id);
        // returns waveform type
        const ma_waveform_type& GetWaveformType(const int id);
        // ADVANCED USAGE, retrieval of raw ma_waveform object
        ma_waveform* GetWaveform(const int id);
    
    public: // noise generation
        
        /**
         * set a noise callback function so your applicaiton
         * can send sound update.
         * 
         * the callback provides two floating point values to
         * override, one for the left channel and one for the
         * right channel. fill them with raw audio data.
         * 
         * for periodic functions, you can reference the fElapsedTime
         * variable to track how much time passed on the previous
         * frame. Accumulate it somewhere to keep track of the total
         * audio time.
         * 
         * if you do not change the output channels, the values
         * previously used wil be played.
         */
        void SetNoiseCallback(std::function<void(float& noiseLeftChannel, float& noiseRightChannel, const float fElapsedTime)>callbackFunc);
        // clears the noise callback and resets the channel values to 0.0
        void ClearNoiseCallback();

    public: // advanced features
        ma_device* GetDevice();
        ma_engine* GetEngine();
        ma_resource_manager* GetResourceManager();
        ma_sound* GetSound(const int id);

    private:
        const int find_or_create_empty_sound_slot();

    private:
        ma_device m_device;
        ma_device_config m_device_config;
        
        ma_resource_manager m_resource_manager;
        ma_resource_manager_config m_resource_manager_config;

        ma_engine m_engine;
        ma_engine_config m_engine_config;
        
        bool m_initialized = false;
        int m_count_play_once_sounds = 0;
        std::vector<Sound*> m_sounds;
        
        std::unordered_map<std::string, SoundFileBuffer> m_sound_file_buffers;
    };
}

#ifdef OLC_PGEX_MINIAUDIO
#undef OLC_PGEX_MINIAUDIO

static constexpr int            DEVICE_CHANNELS     = 2;
static constexpr ma_format      DEVICE_FORMAT       = ma_format_f32;
static constexpr int            DEVICE_SAMPLE_RATE  = 48000;
static constexpr ma_device_type DEVICE_TYPE         = ma_device_type_playback;

void PGEX_MA_LOG(const std::string_view& message = "", std::source_location location = std::source_location::current())
{
#ifdef DEBUG
    std::filesystem::path filePath{location.file_name()};
    
    std::cout << std::format(
        "{}:{} {}(): {}",
        std::string{filePath.filename()},
        location.line(),
        location.function_name(),
        message
    ) << std::endl;
#endif
}

namespace olc
{

#pragma region SoundFileBuffer

    MiniAudio::SoundFileBuffer::SoundFileBuffer()
        : m_engine(nullptr), m_loaded(false)
    {
    }

    MiniAudio::SoundFileBuffer::SoundFileBuffer(ma_engine* engine)
        : m_engine(engine), m_loaded(false)
    {
    }
    
    MiniAudio::SoundFileBuffer::~SoundFileBuffer()
    {
    }

    bool MiniAudio::SoundFileBuffer::Load(const std::string& path, olc::ResourcePack* pack)
    {
        if(m_loaded)
        {
            m_count++;
            PGEX_MA_LOG(std::format("reusing sound file buffer at path: {}", m_path));
            return true;
        }

        if(pack != nullptr)
        {
            PGEX_MA_LOG("loading sound file via olc::ResourcePack");
            
            if(!pack->Loaded())
                throw std::runtime_error{std::format("failed to load sound from: {} - olc::ResourcePack", path)};
                
            m_buffer = pack->GetFileBuffer(path).vMemory;
        }
        else
        {
            PGEX_MA_LOG("loading sound file via ifstream");

            std::ifstream file(path, std::ios::binary | std::ios::ate);
            
            if(!file.is_open())
                throw std::runtime_error{std::format("failed to load sound from: {}", path)};

            m_buffer.resize(file.tellg(), 0);
            file.seekg(0, std::ios::beg);
        
            file.read(m_buffer.data(), m_buffer.size());
        }

        if(ma_resource_manager_register_encoded_data(m_engine->pResourceManager, path.c_str(), m_buffer.data(), m_buffer.size()) != MA_SUCCESS)
            return false;

        m_count = 1;
        m_loaded = true;
        m_path = path;
        
        return true;
    }

    void MiniAudio::SoundFileBuffer::Unload()
    {
        if(!m_loaded)
            throw std::runtime_error{"tried to unload a sound file buffer which isn't loaded"};

        m_count--;
        
        PGEX_MA_LOG(std::format("decreased count of: {} to {}", m_path, m_count));
        
        if(m_count == 0)
        {
            if(ma_resource_manager_unregister_data(m_engine->pResourceManager, m_path.c_str()))
                throw std::runtime_error{"failed to unregister data from the resource manager"};

            m_loaded = false;
            
            PGEX_MA_LOG(std::format("unloaded sound file buffer with path: {}", m_path));
        }
    }

#pragma endregion

#pragma region Sound

    const std::string MiniAudio::Sound::string()
    {
        return std::format(
            "{}, frames({}) seconds({}) play_once({})",
            m_path,
            m_length_in_pcm_frames,
            m_length_in_seconds,
            m_play_once
        );
    }

#pragma endregion

#pragma region Waveform
    MiniAudio::Waveform::Waveform()
    {
    }

    MiniAudio::Waveform::Waveform(const double amplitude, const double frequency, const ma_waveform_type waveformType)
    {
        m_waveform_config = ma_waveform_config_init(
            DEVICE_FORMAT,
            DEVICE_CHANNELS,
            DEVICE_SAMPLE_RATE,
            waveformType,
            amplitude,
            frequency
        );
        
        if(ma_waveform_init(&m_waveform_config, &m_waveform) != MA_SUCCESS)
            throw std::runtime_error{"failed to initialize a waveform"};
        
        m_is_loaded = true;
    }

    void MiniAudio::Waveform::Start()
    {
        m_is_playing = true;
    }

    void MiniAudio::Waveform::Stop()
    {
        m_is_playing = false;
    }

    void MiniAudio::Waveform::Unload()
    {
        m_is_loaded = false;
    }

    bool MiniAudio::Waveform::IsPlaying() const
    {
        return m_is_playing;
    }

    bool MiniAudio::Waveform::IsLoaded()
    {
        return m_is_loaded;
    }

    ma_waveform* MiniAudio::Waveform::Get()
    {
        return &m_waveform;
    }

#pragma endregion

#pragma region MiniAudio

    bool MiniAudio::m_background_playback = false;
    std::vector<float> MiniAudio::m_engine_buffer;
    std::vector<float> MiniAudio::m_waveform_buffer;

    std::vector<MiniAudio::Waveform> MiniAudio::m_waveforms;
    std::function<void(float& out_audio_data_left, float& out_audio_data_right, const float fElapsedTime)> MiniAudio::m_noise_callback;
    float MiniAudio::m_noise_left_channel = 0.0f;
    float MiniAudio::m_noise_right_channel = 0.0f;

    MiniAudio::MiniAudio() : olc::PGEX(true)
    {
        m_device_config = ma_device_config_init(DEVICE_TYPE);
        m_device_config.playback.format = DEVICE_FORMAT;
        m_device_config.playback.channels = DEVICE_CHANNELS;
        m_device_config.sampleRate = DEVICE_SAMPLE_RATE;
        m_device_config.dataCallback = MiniAudio::data_callback;
        m_device_config.pUserData = this;

        if(ma_device_init(NULL, &m_device_config, &m_device) != MA_SUCCESS)
            throw std::runtime_error{"PGEX_MiniAudio: failed to initialize device"};

        m_resource_manager_config = ma_resource_manager_config_init();
        m_resource_manager_config.decodedFormat     = DEVICE_FORMAT;
        m_resource_manager_config.decodedChannels   = DEVICE_CHANNELS;
        m_resource_manager_config.decodedSampleRate = DEVICE_SAMPLE_RATE;
    
    #ifdef __EMSCRIPTEN__
        m_resource_manager_config.jobThreadCount = 0;                           
        m_resource_manager_config.flags |= MA_RESOURCE_MANAGER_FLAG_NON_BLOCKING;
        m_resource_manager_config.flags |= MA_RESOURCE_MANAGER_FLAG_NO_THREADING;
    #endif

        if(ma_resource_manager_init(&m_resource_manager_config, &m_resource_manager) != MA_SUCCESS)
            throw std::runtime_error{"PGEX_MiniAudio: failed to initialize resource manager"};
    
        m_engine_config = ma_engine_config_init();
        m_engine_config.pDevice = &m_device;
        m_engine_config.pResourceManager = &m_resource_manager;
    
        if(ma_engine_init(&m_engine_config, &m_engine) != MA_SUCCESS)
            throw std::runtime_error{"PGEX_MiniAudio: failed to initialize engine"};

        m_initialized = true;
    }

    MiniAudio::~MiniAudio()
    {
        PGEX_MA_LOG("unloading all sounds");

        /**
         * unload all sounds
         */
        for(int i = 0; i < m_sounds.size(); i++)
        {
            if(m_sounds.at(i) == nullptr)
                continue;
            
            UnloadSound(i);
        }

        PGEX_MA_LOG("uninitializing m_engine");
        ma_engine_stop(&m_engine);
        ma_engine_uninit(&m_engine);
        
        PGEX_MA_LOG("uninitializing m_resource_manager");
        ma_resource_manager_uninit(&m_resource_manager);
        
        PGEX_MA_LOG("uninitializing m_device");
        ma_device_stop(&m_device);
        ma_device_uninit(&m_device);
    }

    bool MiniAudio::OnBeforeUserUpdate(float& fElapsedTime)
    {
        #ifdef __EMSCRIPTEN__
        ma_resource_manager_process_next_job(&m_resource_manager);
        #endif
        
        /**
         * look for the sounds we play once
         * if they're done playing unload them
         */
        for(int i = 0; i < m_sounds.size(); i++)
        {
            if(m_sounds.at(i) == nullptr)
                continue;
            
            if(!m_sounds.at(i)->m_play_once)
                continue;
            
            if(ma_sound_is_playing(&m_sounds.at(i)->m_sound))
                continue;
            
            UnloadSound(i);
            m_count_play_once_sounds--;
        }
        
        return false;
    }

    void MiniAudio::data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
    {
        /**
         * Get the instance of the miniaudio pgex for use later
         * in the callback
         */
        MiniAudio* ma = (MiniAudio*)pDevice->pUserData;
        
        /**
         * If the instance of the miniaudio pgex has, for some
         * reason has not been set, bail
         */
        if(ma == nullptr)
            throw std::runtime_error{"unable to access miniaudio pgex instance from data_callback"};

        /**
         * If background playback isn't enabled and the app
         * does not have focus, bail
         */
        if(!MiniAudio::m_background_playback && !pge->IsFocused())
            return;

        /**
         * If required, resize the buffer.
         * 
         * Note:    m_engine_buffer is the buffer we mix other buffers
         *          into prior to copying it to the final output buffer.
         */
        if(m_engine_buffer.size() != (frameCount * DEVICE_CHANNELS))
        {
            m_engine_buffer.resize(frameCount * DEVICE_CHANNELS, 0);
            m_waveform_buffer.resize(frameCount * DEVICE_CHANNELS, 0);
            PGEX_MA_LOG(std::format("had to resize callback buffers to {} bytes", m_engine_buffer.size() * sizeof(float)));
        }

        /**
         * read pcm frames from the engine to the buffer
         */
        ma_engine_read_pcm_frames(&ma->m_engine, m_engine_buffer.data(), frameCount, NULL);

        /**
         * waveforms
         */
        for(Waveform& waveform : MiniAudio::m_waveforms)
        {
            if(!waveform.IsLoaded())
                continue;

            if(waveform.IsPlaying())
            {
                ma_uint64 framesRead;

                /**
                 * read pcm frames from the waveform to the buffer
                 */
                ma_result result = ma_waveform_read_pcm_frames(waveform.Get(), m_waveform_buffer.data(), frameCount, &framesRead);
                
                /**
                 * if reading failed, or read 0 frames, skip
                 */
                if(result != MA_SUCCESS || framesRead == 0)
                    continue;
                
                /**
                 * mix waveform buffer into the engine buffer, simple add
                 */
                for(ma_uint64 i = 0; i < framesRead * DEVICE_CHANNELS; i++)
                {
                    m_engine_buffer[i] += m_waveform_buffer[i];
                }
            }
        }
        
        /**
         * noise generators
         */
        if(m_noise_callback)
        {
            for(ma_uint32 i = 0; i < frameCount; i++)
            {
                m_noise_callback(m_noise_left_channel, m_noise_right_channel, 1.0f / DEVICE_SAMPLE_RATE);

                m_engine_buffer[(i * DEVICE_CHANNELS)] += m_noise_left_channel;
                m_engine_buffer[(i * DEVICE_CHANNELS) + 1] += m_noise_right_channel;
            }
        }
        
        /**
         * clamp the output to a range of -1.0f to 1.0f
         */
        for(auto &sample : m_engine_buffer)
        {
            sample = std::clamp(sample, -1.0f, 1.0f);
        }

        /**
         * Copy the results to the output buffer
         */
        memcpy(pOutput, m_engine_buffer.data(), m_engine_buffer.size() * sizeof(float));
    }

    void MiniAudio::SetBackgroundPlay(bool state)
    {
        MiniAudio::m_background_playback = state;
    }

    const int MiniAudio::LoadSound(const std::string& path, olc::ResourcePack* pack, bool playOnce)
    {
        int id = find_or_create_empty_sound_slot();

        /**
         * if we haven't already create a sound file buffer for this
         * path, let's create it
         */
        if(m_sound_file_buffers.find(path) == m_sound_file_buffers.end())
        {
            m_sound_file_buffers[path] = SoundFileBuffer(&m_engine);
        }
        
        m_sound_file_buffers.at(path).Load(path, pack);
        
        if(ma_sound_init_from_file(&m_engine, path.c_str(), MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC, NULL, NULL, &m_sounds.at(id)->m_sound) != MA_SUCCESS)
            throw std::runtime_error{"PGEX_MiniAudio: failed to initialize a sound"};
        
        /**
         * sounds with the play once flag set will be automatically
         * unloaded when it has finished it's playback.
         */
        m_sounds.at(id)->m_play_once = playOnce;

        /**
         * used to synchronise the underlying sound file buffers
         */
        m_sounds.at(id)->m_path = path;

        /**
         * cache the lengths of the sound because this is an expensive operation, do it once
         */
        ma_sound_get_length_in_pcm_frames(&m_sounds[id]->m_sound, &m_sounds[id]->m_length_in_pcm_frames);
        ma_sound_get_length_in_seconds(&m_sounds[id]->m_sound, &m_sounds[id]->m_length_in_seconds);
        
        PGEX_MA_LOG(m_sounds.at(id)->string());
        return id;
    }

    void MiniAudio::UnloadSound(const int id)
    {
        if(m_sounds.at(id) == nullptr)
            throw std::runtime_error{std::format("tried to unload non-existent sound at id({})", id)};
        
        if(m_sound_file_buffers.find(m_sounds.at(id)->m_path) == m_sound_file_buffers.end())
            throw std::runtime_error{std::format("tried to unload non-existent sound file buffer at path ({})", m_sounds.at(id)->m_path)};
        
        /**
         * if the sound is playing, stop it
         */
        if(ma_sound_is_playing(&m_sounds.at(id)->m_sound))
            ma_sound_stop(&m_sounds.at(id)->m_sound);
        
        /**
         * let miniaudio perform it's uninitialization on the sound
         */
        ma_sound_uninit(&m_sounds.at(id)->m_sound);

        /**
         * unload the sound file buffer
         */
        m_sound_file_buffers.at(m_sounds.at(id)->m_path).Unload();

        /**
         * give the memory back to the os
         */
        delete m_sounds.at(id);

        /**
         * mark this slot as empy
         */
        m_sounds.at(id) = nullptr;
    }

    void MiniAudio::Play(const int id, const bool looping)
    {
        if(ma_sound_is_playing(&m_sounds.at(id)->m_sound))
            return;
        
        if(looping)
            ma_sound_set_looping(&m_sounds.at(id)->m_sound, true);
        
        ma_sound_start(&m_sounds.at(id)->m_sound);
    }
    
    const int MiniAudio::Play(const std::string& path, olc::ResourcePack* pack)
    {
        int id = LoadSound(path, pack, true);
        ma_sound_start(&m_sounds.at(id)->m_sound);
        m_count_play_once_sounds++;
        return id;
    }

    void MiniAudio::Stop(const int id)
    {
        if(!ma_sound_is_playing(&m_sounds.at(id)->m_sound))
            return;
        
        ma_sound_stop(&m_sounds.at(id)->m_sound);
        ma_sound_seek_to_pcm_frame(&m_sounds.at(id)->m_sound, 0);
    }

    void MiniAudio::Pause(const int id)
    {
        if(!ma_sound_is_playing(&m_sounds.at(id)->m_sound))
            return;
        
        ma_sound_stop(&m_sounds.at(id)->m_sound);
    }

    void MiniAudio::Toggle(const int id)
    {
        if(ma_sound_is_playing(&m_sounds.at(id)->m_sound))
        {
            ma_sound_stop(&m_sounds.at(id)->m_sound);
            return;
        }
        
        ma_sound_start(&m_sounds.at(id)->m_sound);
    }

    void MiniAudio::Seek(const int id, const ma_uint64 milliseconds)
    {
        ma_uint64 frame_to_seek_to = (milliseconds * DEVICE_SAMPLE_RATE) / 1000;
        ma_sound_seek_to_pcm_frame(&m_sounds.at(id)->m_sound, frame_to_seek_to);
    }

    void MiniAudio::Seek(const int id, const float& location)
    {
        ma_uint64 frame_to_seek_to = static_cast<ma_uint64>(m_sounds.at(id)->m_length_in_pcm_frames * location);
        ma_sound_seek_to_pcm_frame(&m_sounds.at(id)->m_sound, frame_to_seek_to);
    }

    void MiniAudio::Forward(const int id, const ma_uint64 milliseconds)
    {
        ma_uint64 frame_to_seek_to;

        // get the current position
        ma_sound_get_cursor_in_pcm_frames(&m_sounds.at(id)->m_sound, &frame_to_seek_to);
        
        // calculate the step and add it to the current position
        frame_to_seek_to += ((milliseconds * DEVICE_SAMPLE_RATE) / 1000);

        // seek to the new position
        ma_sound_seek_to_pcm_frame(&m_sounds.at(id)->m_sound, frame_to_seek_to);
    }

    void MiniAudio::Rewind(const int id, const ma_uint64 milliseconds)
    {
        ma_uint64 frame_to_seek_to;

        // get the current position
        ma_sound_get_cursor_in_pcm_frames(&m_sounds.at(id)->m_sound, &frame_to_seek_to);
        
        // calculate the step and subtract it to the current position
        frame_to_seek_to -= ((milliseconds * DEVICE_SAMPLE_RATE) / 1000);

        // seek to the new position
        ma_sound_seek_to_pcm_frame(&m_sounds.at(id)->m_sound, frame_to_seek_to);
    }

    void MiniAudio::SetVolume(const int id, const float& volume)
    {
        ma_sound_set_volume(&m_sounds.at(id)->m_sound, std::clamp(volume, 0.0f, 1.0f));
    }

    void MiniAudio::SetPan(const int id, const float& pan)
    {
        ma_sound_set_pan(&m_sounds.at(id)->m_sound, std::clamp(pan, -1.0f, 1.0f));
    }

    void MiniAudio::SetPitch(const int id, const float& pitch)
    {
        ma_sound_set_pitch(&m_sounds.at(id)->m_sound, std::max({0.0f, pitch}));
    }

    bool MiniAudio::IsPlaying(const int id)
    {
        if(ma_sound_is_playing(&m_sounds.at(id)->m_sound))
            return true;
        
        return false;
    }
    
    ma_uint64 MiniAudio::GetCursorMilliseconds(const int id)
    {
        ma_uint64 cursor;
        ma_sound_get_cursor_in_pcm_frames(&m_sounds.at(id)->m_sound, &cursor);
        return (cursor * 1000) / DEVICE_SAMPLE_RATE;
    }
    
    float MiniAudio::GetCursorFloat(const int id)
    {
        float cursor;
        ma_sound_get_cursor_in_seconds(&m_sounds.at(id)->m_sound, &cursor);
        return cursor / m_sounds.at(id)->m_length_in_seconds;
    }

    int MiniAudio::GetOneOffCount()
    {
        return m_count_play_once_sounds;
    }

    const int MiniAudio::CreateWaveform(const double amplitude, const double frequency, const ma_waveform_type waveformType)
    {
        // attempt to re-use an empty slot
        for(int i = 0; i < m_waveforms.size(); i++)
        {
            if(!m_waveforms.at(i).IsLoaded())
            {
                m_waveforms.at(i) = Waveform{amplitude, frequency, waveformType};
                return i;
            }
        }

        // no empty slots, make more room!
        const int id = static_cast<int>(m_waveforms.size());
        m_waveforms.emplace_back(amplitude, frequency, waveformType);
        return id;
    }

    void MiniAudio::PlayWaveform(const int id)
    {
        m_waveforms.at(id).Start();
    }

    void MiniAudio::SetWaveformAmplitude(const int id, const double amplitude)
    {
        ma_waveform_set_amplitude(m_waveforms.at(id).Get(), amplitude);
    }

    void MiniAudio::SetWaveformFrequency(const int id, const double frequency)
    {
        ma_waveform_set_frequency(m_waveforms.at(id).Get(), frequency);
    }

    void MiniAudio::SetWaveformType(const int id, const ma_waveform_type waveformType)
    {
        ma_waveform_set_type(m_waveforms.at(id).Get(), waveformType);
    }

    void MiniAudio::StopWaveform(const int id)
    {
        m_waveforms.at(id).Stop();
    }

    void MiniAudio::UnloadWaveform(const int id)
    {
        ma_waveform_uninit(m_waveforms.at(id).Get());
        m_waveforms.at(id).Unload();
    }

    ma_waveform* MiniAudio::GetWaveform(const int id)
    {
        return m_waveforms.at(id).Get();
    }

    const bool MiniAudio::IsWaveformPlaying(const int id)
    {
        return m_waveforms.at(id).IsPlaying();
    }

    const double& MiniAudio::GetWaveformAmplitude(const int id)
    {
        return m_waveforms.at(id).Get()->config.amplitude;
    }

    const double& MiniAudio::GetWaveformFrequency(const int id)
    {
        return m_waveforms.at(id).Get()->config.frequency;
    }

    const ma_waveform_type& MiniAudio::GetWaveformType(const int id)
    {
        return m_waveforms.at(id).Get()->config.type;
    }

    void MiniAudio::SetNoiseCallback(std::function<void(float& noiseLeftChannel, float& noiseRightChannel, const float fElapsedTime)>callbackFunc)
    {
        m_noise_callback = callbackFunc;
    }

    void MiniAudio::ClearNoiseCallback()
    {
        MiniAudio::m_noise_left_channel = 0.0f;
        MiniAudio::m_noise_right_channel = 0.0f;
        m_noise_callback = {};
    }

    ma_device* MiniAudio::GetDevice()
    {
        return &m_device;
    }

    ma_engine* MiniAudio::GetEngine()
    {
        return &m_engine;
    }

    ma_resource_manager* MiniAudio::GetResourceManager()
    {
        return &m_resource_manager;
    }

    ma_sound* MiniAudio::GetSound(const int id)
    {
        if(id > (m_sounds.size() - 1))
            return nullptr;

        if(m_sounds.at(id) == nullptr)
            return nullptr;
        
        return &m_sounds.at(id)->m_sound;
    }

    const int MiniAudio::find_or_create_empty_sound_slot()
    {
        /**
         * look for empty slots for re-use
         */
        for(int i = 0; i < m_sounds.size(); i++)
        {
            if(m_sounds[i] == nullptr)
            {
                m_sounds.at(i) = new Sound();
                return i;
            }
                
        }

        /**
         * create a new slot
         */
        int i = static_cast<int>(m_sounds.size());
        m_sounds.push_back(new Sound());
        return i;
    }

#pragma endregion

} // olc

#endif