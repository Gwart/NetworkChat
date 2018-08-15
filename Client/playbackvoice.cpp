#include "playbackvoice.h"

PlaybackVoice::PlaybackVoice()
{
	volume = 0xFFFFFFFF;
	samplesPs = 8000;
	
	pFormat.wFormatTag = WAVE_FORMAT_PCM;     
	pFormat.nChannels = 1;                    
	pFormat.wBitsPerSample = 16;              
	pFormat.nSamplesPerSec = samplesPs;
	pFormat.nBlockAlign = (pFormat.nChannels * pFormat.wBitsPerSample) / 8;   	
	pFormat.nAvgBytesPerSec = samplesPs * pFormat.nBlockAlign;
	
	waveOutOpen(&hWaveOut, 
	WAVE_MAPPER, &pFormat, 0, 0, WAVE_FORMAT_DIRECT);
}

PlaybackVoice::~PlaybackVoice()
{
	waveOutReset(hWaveOut);
	waveOutClose(hWaveOut);
}

void PlaybackVoice::Play(char* buffer, size_t bufferSize)
{
		WaveOutHdr1.lpData = (LPSTR)buffer;
		WaveOutHdr1.dwBufferLength = bufferSize;
		WaveOutHdr1.dwFlags = 0;
		
		waveOutPrepareHeader(hWaveOut, &WaveOutHdr1, sizeof(WAVEHDR));
		waveOutWrite(hWaveOut, &WaveOutHdr1, sizeof(WAVEHDR));
		
		waveOutUnprepareHeader(hWaveOut, &WaveOutHdr1, sizeof(WAVEHDR));
}

MMRESULT PlaybackVoice::SetVolume(DWORD volume)
{
	return waveOutSetVolume(hWaveOut, volume);
}

MMRESULT PlaybackVoice::SetVolumeMax()
{
	return waveOutSetVolume(hWaveOut, volume);
}