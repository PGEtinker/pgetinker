.PHONY: all clean miniaudio olcPGEX_Gamepad olcPGEX_MiniAudio olcPixelGameEngine olcSoundWaveEngine raylib

all: miniaudio olcPGEX_Gamepad olcPGEX_MiniAudio olcPixelGameEngine olcSoundWaveEngine raylib

miniaudio:
	$(MAKE) -C miniaudio

olcPGEX_Gamepad:
	$(MAKE) -C olcPGEX_Gamepad

olcPGEX_MiniAudio:
	$(MAKE) -C olcPGEX_MiniAudio

olcPixelGameEngine:
	$(MAKE) -C olcPixelGameEngine

olcSoundWaveEngine:
	$(MAKE) -C olcSoundWaveEngine

raylib:
	$(MAKE) -C raylib

clean:
	$(MAKE) -C miniaudio clean
	$(MAKE) -C olcPGEX_MiniAudio clean
	$(MAKE) -C olcPGEX_Gamepad clean
	$(MAKE) -C olcPixelGameEngine clean
	$(MAKE) -C olcSoundWaveEngine clean
	$(MAKE) -C raylib clean
	
