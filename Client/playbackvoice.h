#pragma once
#ifndef _PLAYBACKVOICE_H_
#define _PLAYBACKVOICE_H_
#include <windows.h>

class PlaybackVoice
{
	public:
		
		PlaybackVoice();
		~PlaybackVoice();
		void Play(char* buffer, size_t bufferSize);
		MMRESULT SetVolume(DWORD volume);
		MMRESULT SetVolumeMax();
		
	private:
		
		HWAVEOUT hWaveOut;
		WAVEFORMATEX pFormat;
		DWORD volume;
		WAVEHDR WaveOutHdr1;
		WAVEHDR WaveOutHdr2;
		unsigned short int samplesPs;
};

#endif