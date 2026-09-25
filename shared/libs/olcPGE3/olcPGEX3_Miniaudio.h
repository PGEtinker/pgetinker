#pragma once
/*
	olcPGEX3_MiniAudio.h

	+-------------------------------------------------------------+
	|         OneLoneCoder Pixel Game Engine Extension            |
	|                     Miniaudio v3.0                          |
	+-------------------------------------------------------------+

    What is this?
	~~~~~~~~~~~~~
    This extension abstracts the very robust and powerful miniaudio
    library. It provides simple loading and playback of WAV and MP3
    files. Because it's built on top of miniaudio, it requires next
    to no addictional build configurations in order to be built
    for cross-platform.

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2023-2026 Moros Smith <moros1138@gmail.com>

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
	Homepage:	https://moros1138.com
*/
#if defined(OLC_MULTIHEADER)
#include "olcpge3.h"
#else
#include "olcPixelGameEngine3.h"
#endif

#ifdef OLC_PGEX3_MINIAUDIO
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#endif

#include <cstring>
#include <fstream>
#include <functional>
#include <string>
#include <vector>
#include <span>

namespace olc::ext::Miniaudio
{
    class AudioEngine;

#pragma region Sound

	class Sound
	{
		friend class AudioEngine;
	public:
		Sound() = default;
		~Sound();

	public: // Loaders
		// Create an image resource based on an image file asset on disk
		bool CreateSoundFromFile(const std::string& sFileName, AudioEngine* pgex, uint32_t nNumVoices);
		// Create an image resource based on an image file asset in memory
		bool CreateSoundFromMemory(const uint8_t* data, const size_t bytes, AudioEngine* pgex, uint32_t nNumVoices);
		// Create an image resource based on an image file asset in memory
		bool CreateSoundFromMemory(const std::vector<uint8_t>& data, AudioEngine* pgex, uint32_t nNumVoices);
	private:
		// performs the necessary unloading of the parts of this sound
		void DestroySound();

	private: // loader function common to all loaders
		bool _internalSoundLoader();

	
    public: // playback routines
        // plays a sound, can be set to loop
        void Play(const bool looping = false);
        // stops a sound, rewinds to beginning
        void Stop();
        // pauses a sound, does not change position
        void Pause();
        // toggle between play and pause
        void Toggle();

    public: // seeking controls
        // seek to the provided position in the sound, by milliseconds
        void Seek(const ma_uint64 milliseconds);
        // seek to the provided position in the sound, by float 0.f is beginning, 1.0f is end
        void Seek(const float& location);
        // seek forward from current position by the provided time
        void Forward(const ma_uint64 milliseconds);
        // seek forward from current position by the provided time
        void Rewind(const ma_uint64 milliseconds);

    public: // expression controls
        // set volume of a sound, 0.0f is mute, 1.0f is full
        void SetVolume(const float& volume);
        // set pan of a sound, -1.0f is left, 1.0f is right, 0.0f is center
        void SetPan(const float& pan);
        // set pitch of a sound, 1.0f is normal
        void SetPitch(const float& pitch);

    public: // misc information
        // determine if a sound is playing
        bool IsPlaying();
        // gets the current position in the sound, in milliseconds
        ma_uint64 GetCursor();
        // gets the current position in the sound, as a float between 0.0f and 1.0f
        float GetCursorFloat();
		// determine if this sound has been loaded successfully
		bool IsLoaded() const;
	public: // advanced usage
		ma_sound* GetMASound();

	private:
		// pointer to the calling pgex
		AudioEngine* m_pgex;
		// contains the sound file in memory
		std::vector<uint8_t> m_buffer;
		// the base sound from which the voices are copied
		ma_sound m_base_sound{0};
		// the voices of this sound
		std::vector<ma_sound> m_voices;
		// the id of this sound, used to derive virtual path
		uint32_t m_id{0};
		// the virtual path for the resource manager, usually sound/<id>
		std::string m_virtual_path{""};
	private: // sound status
		// has the sound been loaded successfully
		bool m_is_loaded{false};
		// is the sound in a paused state
		bool m_is_paused{false};
		// number of voices
		uint32_t m_num_voices{8};
		// track the current voice
		uint32_t m_current_voice{0};
	
	private: // info
		// the length of this sound in pcm frames
		ma_uint64 m_length_in_pcm_frames{0};
		// the length of this sound in seconds
		float m_length_in_seconds{0.0f};

	private: // globals, has an effect on all sounds
		// id tracker allows us to ensure every sound has a unique id
		static uint32_t m_id_tracker;
	};
#pragma endregion

#pragma region Waveform
	
	
	class Waveform
	{
		friend class AudioEngine;

	public:
		enum class Type
		{
			Sine,
			Square,
			Triangle,
			Sawtooth
		};
	
	public: // lifecycle
		Waveform() = default;
	
		bool CreateWaveform(Waveform& waveform, const Type type, const double amplitude, const double frequency, AudioEngine* pgex);
	private: 
		void DestroyWaveform();
	
	public: // playback
		void Play();
		void Stop();
	
	public: // configuration
		void SetAmplitude(const double amplitude);
		void SetFrequency(const double frequency);
		void SetType(const Type type);

		bool IsPlaying() const;
		bool IsLoaded() const;
	
		ma_waveform& Get();

	private:
		bool m_is_loaded{false};
		float m_gain     = 0.0f;  // current gain
		float m_target   = 0.0f;  // 0.0 = stopped, 1.0 = playing
		float m_rampStep = 0.0f;  // set once at init: 1.0f / (sampleRate * 0.010f)

		ma_waveform m_waveform;
		ma_waveform_config m_waveform_config;

		AudioEngine* m_pgex{nullptr};
	};

#pragma endregion

#pragma region Miniaudio

	class AudioEngine : public olc::PGESystemExtension
    {
		friend class Sound;
		friend class Waveform;

	public:
		struct Config
		{
			// device: number of channels. default(2)
			int DeviceChannels{2};
			// device: format of the audio data. default(ma_format_f32)
			ma_format DeviceFormat{ma_format_f32};
			// device: sample rate. default(48000)
			int DeviceSampleRate{ma_standard_sample_rate_48000};
			// device: type of device. default(ma_device_type_playback)
			ma_device_type DeviceType{ma_device_type_playback};
			// Miniaudio: is background play enabled? default(false)
			bool BackgroundPlay{false};
			// Logging: is logging verbose? default(false)
			bool Verbose{false};
		};

		// configure the audio engine, see struct Config
		void Configure(const Config& cfg);
		// enable playback when the application window does not have focus.
		void EnableBackgroundPlayback();
		// disable playback when the application window does not have focus.
		void DisableBackgroundPlayback();

	public: // Callback
		static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

	public: // Sounds
		// Create a sound resource based on a sound file asset on disk
		bool CreateSoundFromFile(Sound& sound, const std::string& sFileName, uint32_t nNumVoices = 8);
		// Create a sound resource based on a sound file asset in memory
		bool CreateSoundFromMemory(Sound& sound, const uint8_t* data, const size_t bytes, uint32_t nNumVoices = 8);
		bool CreateSoundFromMemory(Sound& sound, const std::vector<uint8_t>& data, uint32_t nNumVoices = 8);
		void DestroySound(Sound& sound);

	public: // Waveforms
		bool CreateWaveform(Waveform& waveform, const Waveform::Type type, const double amplitude, const double frequency);
		void DestroyWaveform(Waveform& waveform);
	public: // Synth
		void SetSynthCallback(std::function<void(float& fLeftChannel, float& fRightChannel, float fElapsedTime)> callback);
		void ClearSynthCallback();
	public: // Absolute POWER!!
		void SetDataCallback(std::function<void(float* pFramesOut, ma_uint64 frameCount)> callback);
		void ClearDataCallback();

	public: // getters
		// 
		ma_device& GetDevice();
		ma_engine& GetEngine();
		ma_resource_manager& GetResourceManager();

		int GetDeviceChannels() const;
		ma_format GetDeviceFormat() const;
		int GetDeviceSampleRate() const;
		ma_device_type GetDeviceType() const;
		
	public:
        AudioEngine();
		~AudioEngine();

		virtual bool OnInstall([[maybe_unused]] olc::PixelGameEngine* pge);
		virtual bool OnBeforeUserCreate([[maybe_unused]] olc::PixelGameEngine* pge);
		virtual bool OnAfterUserCreate([[maybe_unused]] olc::PixelGameEngine* pge);
		virtual bool OnBeforeSystemUpdate([[maybe_unused]] olc::PixelGameEngine* pge, [[maybe_unused]] float fElapsedTime);
		virtual bool OnAfterSystemUpdate([[maybe_unused]] olc::PixelGameEngine* pge, [[maybe_unused]] float fElapsedTime);
		
	private:
        Config m_cfg;
		ma_device m_device;
        ma_device_config m_device_config;
        
        ma_resource_manager m_resource_manager;
        ma_resource_manager_config m_resource_manager_config;

        ma_engine m_engine;
        ma_engine_config m_engine_config;
		std::vector<float> m_waveform_buffer;
		
		// synth callback function
		std::function<void(float& out_data_channel_left, float& out_data_channel_right, const float fElapsedTime)> m_synth_callback;
		// data callback function
		std::function<void(float* pFramesOut, ma_uint64 frameCount)> m_data_callback;

		// track sounds and waveforms
		std::vector<Sound*> m_sounds;
		std::vector<Waveform*> m_waveforms;

		bool m_is_initialized{false};
		olc::PixelGameEngine* m_pge{nullptr};
    };
}
#pragma endregion


#if defined(OLC_PGEX3_MINIAUDIO)
#undef OLC_PGEX3_MINIAUDIO

namespace olc::ext::Miniaudio
{

#pragma region Sound

	uint32_t Sound::m_id_tracker = 0;
	
	Sound::~Sound()
	{
		DestroySound();
	}

	bool Sound::CreateSoundFromFile(const std::string& sFileName, AudioEngine* pgex, uint32_t nNumVoices)
	{
#if OLC_HOST == OLC_HOST_ANDROID
		AAsset* pAsset = AAssetManager_open(
			olc::host::Host_Android::androidApp->activity->assetManager,
			sFileName.c_str(),
			AASSET_MODE_BUFFER
		);
		
		if (pAsset == nullptr)
			return false;

		off_t size = AAsset_getLength(pAsset);
		m_buffer.resize(size);
		AAsset_read(pAsset, m_buffer.data(), size);
		AAsset_close(pAsset);
#else
		std::ifstream f(sFileName, std::ios::binary | std::ios::ate);
		if(f.fail())
			return false;
		m_buffer.resize(f.tellg());
		f.seekg(0);
		f.read(reinterpret_cast<char*>(m_buffer.data()), m_buffer.size());
		f.close();
#endif
		m_pgex = pgex;
		m_num_voices = nNumVoices;
		return _internalSoundLoader();
	}

	bool Sound::CreateSoundFromMemory(const uint8_t* data, const size_t bytes, AudioEngine* pgex, uint32_t nNumVoices)
	{
		if(!data) return false;
		if(bytes <= 0) return false;

		m_buffer.resize(bytes);
		uint8_t* result = reinterpret_cast<uint8_t*>(std::memcpy(m_buffer.data(), data, m_buffer.size()));
		if(result == m_buffer.data())
			return false;

		m_pgex = pgex;
		m_num_voices = nNumVoices;
		return _internalSoundLoader();
	}

	bool Sound::CreateSoundFromMemory(const std::vector<uint8_t>& data, AudioEngine* pgex, uint32_t nNumVoices)
	{
		if(data.size() <= 0) return false;
		m_buffer = data;
		m_pgex = pgex;
		m_num_voices = nNumVoices;
		return _internalSoundLoader();
	}

	void Sound::DestroySound()
	{
		if(!m_is_loaded) return;

		for(auto& v : m_voices)
		{
			if(ma_sound_is_playing(&v))
				ma_sound_stop(&v);

			ma_sound_uninit(&v);
		}
		m_voices.clear();
		ma_sound_uninit(&m_base_sound);
		ma_resource_manager_unregister_data(ma_engine_get_resource_manager(&m_pgex->GetEngine()), m_virtual_path.c_str());
		m_is_loaded = false;
	}

	bool Sound::_internalSoundLoader()
	{
		ma_result result;
		m_id = ++m_id_tracker;
		m_virtual_path = "sound/" + std::to_string(m_id); 
		
		result = ma_resource_manager_register_encoded_data(
			ma_engine_get_resource_manager(&m_pgex->GetEngine()),
			m_virtual_path.c_str(),
			m_buffer.data(), m_buffer.size()
		);
		
		if(result != MA_SUCCESS)
			return false;

		ma_fence fence;
		result = ma_fence_init(&fence);

		if(result != MA_SUCCESS)
		{
			ma_resource_manager_unregister_data(ma_engine_get_resource_manager(&m_pgex->GetEngine()), m_virtual_path.c_str());
			return false;
		}

		result = ma_sound_init_from_file(
			&m_pgex->GetEngine(),
			m_virtual_path.c_str(),
			MA_SOUND_FLAG_DECODE,
			nullptr,
			&fence,
			&m_base_sound
		);
		
		if(result != MA_SUCCESS)
		{
			ma_resource_manager_unregister_data(ma_engine_get_resource_manager(&m_pgex->GetEngine()), m_virtual_path.c_str());
			return false;
		}
		
		m_voices.resize(m_num_voices);
		for(int i = 0; i < m_num_voices; ++i)
		{
			result = ma_sound_init_copy(&m_pgex->GetEngine(), &m_base_sound, 0, nullptr, &m_voices[i]);
			if(result != MA_SUCCESS)
				break;
		}
		
		// if the last result out of that loop isn't success, we failed
		if(result != MA_SUCCESS)
		{
			for(auto& v : m_voices)
				ma_sound_uninit(&v);
			ma_sound_uninit(&m_base_sound);
			ma_resource_manager_unregister_data(ma_engine_get_resource_manager(&m_pgex->GetEngine()), m_virtual_path.c_str());
			return false;
		}
		
		// wait here until the sound is fully loaded and dedoded
		ma_fence_wait(&fence);
		ma_fence_uninit(&fence);

		ma_sound_get_length_in_pcm_frames(&m_base_sound, &m_length_in_pcm_frames);
		ma_sound_get_length_in_seconds(&m_base_sound, &m_length_in_seconds);

		m_is_loaded = true;
        return true;
	}

	// plays a sound, can be set to loop
	void Sound::Play(const bool looping)
	{
		if(!m_is_paused)
			m_current_voice = (m_current_voice + 1) % m_num_voices;
		
		ma_sound_set_looping(&m_voices[m_current_voice], looping);
		ma_sound_seek_to_pcm_frame(&m_voices[m_current_voice], 0);
		ma_sound_start(&m_voices[m_current_voice]);
		m_is_paused = false;
	}
	
	// stops a sound, rewinds to beginning
	void Sound::Stop()
	{
		if(!ma_sound_is_playing(&m_voices[m_current_voice]))
			return;
		ma_sound_stop(&m_voices[m_current_voice]);
		ma_sound_seek_to_pcm_frame(&m_voices[m_current_voice], 0);
	}

	// pauses a sound, does not change position
	void Sound::Pause()
	{
		if(!ma_sound_is_playing(&m_voices[m_current_voice]))
			return;
		
		ma_sound_stop(&m_voices[m_current_voice]);
		m_is_paused = true;
	}
	
	// toggle between play and pause
	void Sound::Toggle()
	{
		if(ma_sound_is_playing(&m_voices[m_current_voice]))
		{
			ma_sound_stop(&m_voices[m_current_voice]);
			m_is_paused = true;
			return;
		}
		
		ma_sound_start(&m_voices[m_current_voice]);
		m_is_paused = false;
	}

	// seek to the provided position in the sound, by milliseconds
	void Sound::Seek(const ma_uint64 milliseconds)
	{
        ma_uint64 frame_to_seek_to = (milliseconds * m_pgex->GetDeviceSampleRate()) / 1000;
        ma_sound_seek_to_pcm_frame(&m_voices[m_current_voice], frame_to_seek_to);
	}
	
	// seek to the provided position in the sound, by float 0.f is beginning, 1.0f is end
	void Sound::Seek(const float& location)
	{
		ma_uint64 frame_to_seek_to = static_cast<ma_uint64>(m_length_in_pcm_frames * location);
		ma_sound_seek_to_pcm_frame(&m_voices[m_current_voice], frame_to_seek_to);
	}
	
	// seek forward from current position by the provided time
	void Sound::Forward(const ma_uint64 milliseconds)
	{
        ma_uint64 frame_to_seek_to;

        // get the current position
        ma_sound_get_cursor_in_pcm_frames(&m_voices[m_current_voice], &frame_to_seek_to);
        
        // calculate the step and add it to the current position
        frame_to_seek_to += ((milliseconds * m_pgex->GetDeviceSampleRate()) / 1000);

        // seek to the new position
        ma_sound_seek_to_pcm_frame(&m_voices[m_current_voice], frame_to_seek_to);
	}
	
	// seek forward from current position by the provided time
	void Sound::Rewind(const ma_uint64 milliseconds)
	{
        ma_uint64 frame_to_seek_to;

        // get the current position
        ma_sound_get_cursor_in_pcm_frames(&m_voices[m_current_voice], &frame_to_seek_to);
        
        // calculate the step and add it to the current position
        frame_to_seek_to -= ((milliseconds * m_pgex->GetDeviceSampleRate()) / 1000);

        // seek to the new position
        ma_sound_seek_to_pcm_frame(&m_voices[m_current_voice], frame_to_seek_to);		
	}

	// set volume of a sound, 0.0f is mute, 1.0f is full
	void Sound::SetVolume(const float& volume)
	{
		for(auto& v : m_voices)
		{
			ma_sound_set_volume(&v, std::clamp(volume, 0.0f, 1.0f));
		}
	}

	// set pan of a sound, -1.0f is left, 1.0f is right, 0.0f is center
	void Sound::SetPan(const float& pan)
	{
		for(auto& v : m_voices)
		{
			ma_sound_set_pan(&v, std::clamp(pan, -1.0f, 1.0f));
		}
	}
	
	// set pitch of a sound, 1.0f is normal
	void Sound::SetPitch(const float& pitch)
	{
		for(auto& v : m_voices)
		{
			ma_sound_set_pitch(&v, std::max({0.0f, pitch}));
		}
	}

	// determine if a sound is playing
	bool Sound::IsPlaying()
	{
		for(auto& v : m_voices)
		{
			if(ma_sound_is_playing(&v))
				return true;
		}
		return false;
	}
	
	// gets the current position in the sound, in milliseconds
	ma_uint64 Sound::GetCursor()
	{
        ma_uint64 cursor;
        ma_sound_get_cursor_in_pcm_frames(&m_voices[m_current_voice], &cursor);
        return (cursor * 1000) / m_pgex->GetDeviceSampleRate();
	}
	
	// gets the current position in the sound, as a float between 0.0f and 1.0f
	float Sound::GetCursorFloat()
	{
        float cursor;
		ma_sound_get_cursor_in_seconds(&m_voices[m_current_voice], &cursor);
		return cursor / m_length_in_seconds;
	}

	// determine if this sound has been loaded successfully
	bool Sound::IsLoaded() const
	{
		return m_is_loaded;
	}

	ma_sound* Sound::GetMASound()
	{
		// realistically, one wouldn't call this unless it was loaded
		if(!IsLoaded())
			return nullptr;
		
		// if we're not currently playing, get the pointer of the next voice
		if(!IsPlaying())
			return &m_voices[(m_current_voice + 1) % m_num_voices];
		
		// if we're playing, get the pointer of the current voice
		return &m_voices[m_current_voice];
	}

#pragma endregion


#pragma region Waveform

	bool Waveform::CreateWaveform(Waveform& waveform, const Type type, const double amplitude, const double frequency, AudioEngine* pgex)
	{
		m_pgex = pgex;
		m_waveform_config = ma_waveform_config_init(
				m_pgex->GetDeviceFormat(),
				m_pgex->GetDeviceChannels(),
				m_pgex->GetDeviceSampleRate(),
				static_cast<ma_waveform_type>(type),
				amplitude,
				frequency
		);
		
		m_rampStep = 1.0f / (m_pgex->GetDeviceSampleRate() * 0.02f);

		if(ma_waveform_init(&m_waveform_config, &m_waveform) != MA_SUCCESS)
		{
			return false;
		}
		
		m_is_loaded = true;
		return true;
	}
	
	void Waveform::DestroyWaveform()
	{
		ma_waveform_uninit(&m_waveform);
		m_is_loaded = false;
	}
	
	void Waveform::Play()
	{
		if(!IsLoaded())
			return;

		m_target = 1.0f;
	}
	
	void Waveform::Stop()
	{
		if(!IsLoaded())
			return;
		
		m_target = 0.0f;
	}

	
	void Waveform::SetAmplitude(const double amplitude)
	{
		if(!IsLoaded())
			return;
		ma_waveform_set_amplitude(&m_waveform, amplitude);
	}

	void Waveform::SetFrequency(const double frequency)
	{
		if(!IsLoaded())
			return;
		ma_waveform_set_frequency(&m_waveform, frequency);
	}
	
	void Waveform::SetType(const Type type)
	{
		if(!IsLoaded())
			return;
		ma_waveform_set_type(&m_waveform, static_cast<ma_waveform_type>(type));
	}

	bool Waveform::IsPlaying() const
	{
		return m_target > 0.0f || m_gain > 0.0f;
	}

	bool Waveform::IsLoaded() const
	{
		return m_is_loaded;
	}

	ma_waveform& Waveform::Get()
	{
		return m_waveform;
	}

#pragma endregion

#pragma region Miniaudio

	AudioEngine::AudioEngine()
    {
    }

	AudioEngine::~AudioEngine()
    {
		if(m_is_initialized)
		{
			for(auto sound : m_sounds)
				sound->DestroySound();
			
			m_sounds.clear();
			ma_resource_manager_uninit(&m_resource_manager);

			ma_engine_stop(&m_engine);
			ma_engine_uninit(&m_engine);
			
			
			ma_device_stop(&m_device);
			ma_device_uninit(&m_device);
		}
    }
	
	void AudioEngine::Configure(const Config& cfg)
	{
		if(m_is_initialized)
		{
			std::cerr << "olcPGEX3_miniaudio: Configure called after initialized.\n";
			return;
		}

		m_cfg = cfg;
	}

	void AudioEngine::EnableBackgroundPlayback()
	{
		m_cfg.BackgroundPlay = true;
	}
	
	void AudioEngine::DisableBackgroundPlayback()
	{
		m_cfg.BackgroundPlay = false;
	}

    void AudioEngine::data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
    {
		AudioEngine* ma = (AudioEngine*)pDevice->pUserData;
        if(ma == nullptr)
            throw std::runtime_error{"unable to access miniaudio pgex instance from data_callback"};

        if(!ma->m_cfg.BackgroundPlay && !ma->m_pge->IsFocused())
			return;

		// with great power comes...
		if(ma->m_data_callback)
		{
			ma->m_data_callback((float*)pOutput, frameCount);
			return;
		}

		std::span<float> engineBuffer((float*)pOutput, frameCount * ma->GetDeviceChannels());
        ma_engine_read_pcm_frames(&ma->m_engine, engineBuffer.data(), frameCount, NULL);
		
		// resize, if required. frameCount is not guaranteed not to change.
        if(ma->m_waveform_buffer.size() != (frameCount * ma->GetDeviceChannels()))
        {
            ma->m_waveform_buffer.resize(frameCount * ma->GetDeviceChannels(), 0);
        }

		// waveforms
		for(auto& waveform : ma->m_waveforms)
		{
			if (!waveform->IsPlaying())
				continue;


			ma_waveform_read_pcm_frames(&waveform->m_waveform, ma->m_waveform_buffer.data(), frameCount, NULL);

			for(int frame = 0; frame < frameCount; ++frame)
			{
				// Ramp gain toward target one step per frame
				if (waveform->m_gain < waveform->m_target)
					waveform->m_gain = std::min(waveform->m_gain + waveform->m_rampStep, waveform->m_target);
				else if (waveform->m_gain > waveform->m_target)
					waveform->m_gain = std::max(waveform->m_gain - waveform->m_rampStep, waveform->m_target);

				for(int channel = 0; channel < ma->GetDeviceChannels(); ++channel)
				{
					int i = frame * ma->GetDeviceChannels() + channel;
					engineBuffer[i] += ma->m_waveform_buffer[i] * waveform->m_gain;
				}
			}
		}

		// synth function
		if(ma->m_synth_callback)
		{
            for(ma_uint32 i = 0; i < frameCount; i++)
            {
                float left, right;
                ma->m_synth_callback(left, right, 1.0f / ma->GetDeviceSampleRate());

                engineBuffer[(i * ma->GetDeviceChannels())]     += left;
                engineBuffer[(i * ma->GetDeviceChannels()) + 1] += right;
            }
		}

		// limiter
		static float envelope = 1.0f;
		
		for(int i = 0; i < engineBuffer.size(); i++)
		{
			float peak = fabsf(engineBuffer[i]);
			
			if (peak > 1.0f)
				envelope = fminf(envelope, 1.0f / peak); // duck the gain
			else
				envelope = fminf(1.0f, envelope * 1.001f); // slowly recover
			// limit the output
			engineBuffer[i] *= envelope;
		}
    }

	bool AudioEngine::CreateSoundFromFile(Sound& sound, const std::string& sFileName, uint32_t nNumVoices)
	{
		m_sounds.push_back(&sound);
		return sound.CreateSoundFromFile(sFileName, this, nNumVoices);
	}
	
	bool AudioEngine::CreateSoundFromMemory(Sound& sound, const uint8_t* data, const size_t bytes, uint32_t nNumVoices)
	{
		m_sounds.push_back(&sound);
		return sound.CreateSoundFromMemory(data, bytes, this, nNumVoices);
	}
	
	bool AudioEngine::CreateSoundFromMemory(Sound& sound, const std::vector<uint8_t>& data, uint32_t nNumVoices)
	{
		m_sounds.push_back(&sound);
		return sound.CreateSoundFromMemory(data, this, nNumVoices);
	}
	
	void AudioEngine::DestroySound(Sound& sound)
	{
		sound.DestroySound();
		
		m_sounds.erase(
			std::remove_if(
				m_sounds.begin(),
				m_sounds.end(),
				[&](Sound* s) { return (s == &sound); }
			),
			m_sounds.end()
		);
	}

	bool AudioEngine::CreateWaveform(Waveform& waveform, const Waveform::Type type, const double amplitude, const double frequency)
	{
		m_waveforms.push_back(&waveform);
		return waveform.CreateWaveform(waveform, type, amplitude, frequency, this);
	}

	void AudioEngine::DestroyWaveform(Waveform& waveform)
	{
		waveform.DestroyWaveform();
		m_waveforms.erase(
			std::remove_if(
				m_waveforms.begin(),
				m_waveforms.end(),
				[&](Waveform* w) { return (w == &waveform); }
			),
			m_waveforms.end()
		);
	}

    void AudioEngine::SetSynthCallback(std::function<void(float& fLeftChannel, float& fRightChannel, float fElapsedTime)> callback)
    {
        m_synth_callback = callback;
    }

    void AudioEngine::ClearSynthCallback()
    {
        m_synth_callback = {};
    }	

	void AudioEngine::SetDataCallback(std::function<void(float* pFramesOut, ma_uint64 frameCount)> callback)
	{
		m_data_callback = callback;
	}

	void AudioEngine::ClearDataCallback()
	{
		m_data_callback = {};
	}
	
	ma_device& AudioEngine::GetDevice()
	{
		return m_device;
	}

	ma_engine& AudioEngine::GetEngine()
	{
		return m_engine;
	}

	ma_resource_manager& AudioEngine::GetResourceManager()
	{
		return m_resource_manager;
	}

	int AudioEngine::GetDeviceChannels() const
	{
		return m_cfg.DeviceChannels;
	}

	ma_format AudioEngine::GetDeviceFormat() const
	{
		return m_cfg.DeviceFormat;
	}
	
	int AudioEngine::GetDeviceSampleRate() const
	{
		return m_cfg.DeviceSampleRate;
	}
	
	ma_device_type AudioEngine::GetDeviceType() const
	{
		return m_cfg.DeviceType;
	}

	bool AudioEngine::OnInstall([[maybe_unused]] olc::PixelGameEngine* pge)
	{
        m_pge = pge;

		m_device_config = ma_device_config_init(GetDeviceType());
        m_device_config.playback.format = GetDeviceFormat();
        m_device_config.playback.channels = GetDeviceChannels();
        m_device_config.sampleRate = GetDeviceSampleRate();
        m_device_config.dataCallback = AudioEngine::data_callback;
        m_device_config.pUserData = this;
		
        if(ma_device_init(NULL, &m_device_config, &m_device) != MA_SUCCESS)
		{
            std::cerr << "PGEX3_Miniaudio: failed to initialize device\n";
			return false;
		}

        m_resource_manager_config = ma_resource_manager_config_init();
        m_resource_manager_config.decodedFormat     = GetDeviceFormat();
        m_resource_manager_config.decodedChannels   = GetDeviceChannels();
        m_resource_manager_config.decodedSampleRate = GetDeviceSampleRate();
    
    #ifdef __EMSCRIPTEN__
        m_resource_manager_config.jobThreadCount = 0;                           
        m_resource_manager_config.flags |= MA_RESOURCE_MANAGER_FLAG_NON_BLOCKING;
        m_resource_manager_config.flags |= MA_RESOURCE_MANAGER_FLAG_NO_THREADING;
    #endif

        if(ma_resource_manager_init(&m_resource_manager_config, &m_resource_manager) != MA_SUCCESS)
		{
            std::cerr <<"PGEX3_Miniaudio: failed to initialize resource manager\n";
			return false;
		}
    
        m_engine_config = ma_engine_config_init();
        m_engine_config.pDevice = &m_device;
        m_engine_config.pResourceManager = &m_resource_manager;
    
        if(ma_engine_init(&m_engine_config, &m_engine) != MA_SUCCESS)
		{
			std::cerr << "PGEX3_Miniaudio: failed to initialize engine\n";
			return false;
		}
		m_is_initialized = true;
		return true;
	}
	
	bool AudioEngine::OnBeforeUserCreate([[maybe_unused]] olc::PixelGameEngine* pge)
	{
		return true;
	}
	
	bool AudioEngine::OnAfterUserCreate([[maybe_unused]] olc::PixelGameEngine* pge)
	{
		return true;
	}
	
	bool AudioEngine::OnBeforeSystemUpdate([[maybe_unused]] olc::PixelGameEngine* pge, [[maybe_unused]] float fElapsedTime)
	{
        #if OLC_HOST == OLC_HOST_EMSCRIPTEN
        ma_resource_manager_process_next_job(&m_resource_manager);
        #endif

		return true;
	}
	
	bool AudioEngine::OnAfterSystemUpdate([[maybe_unused]] olc::PixelGameEngine* pge, [[maybe_unused]] float fElapsedTime)
	{
		return true;
	}
#pragma endregion

}
#endif
