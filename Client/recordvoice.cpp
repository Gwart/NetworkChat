#include "recordvoice.h"



RecordVoice::RecordVoice()
{
	pFormat.wFormatTag = WAVE_FORMAT_PCM;     
	pFormat.nChannels = 1;                    
	pFormat.wBitsPerSample = 16;              
	pFormat.nSamplesPerSec = samplesPs;
	pFormat.nBlockAlign = (pFormat.nChannels * pFormat.wBitsPerSample) / 8;   	
	pFormat.nAvgBytesPerSec = samplesPs * pFormat.nBlockAlign;
	pFormat.cbSize = 0;
	
	waveInOpen(&hWaveIn, WAVE_MAPPER, &pFormat, 0, 0, WAVE_FORMAT_DIRECT);
}

RecordVoice::~RecordVoice()
{
	waveInStop(hWaveIn);
	waveInReset(hWaveIn);
	waveInClose(hWaveIn);
}

int RecordVoice::Record(char* buf, size_t bufferSize)
{
	waveInHdr.lpData = (LPSTR)buf;
    waveInHdr.dwBufferLength = bufferSize;
    waveInHdr.dwBytesRecorded = 0;
    waveInHdr.dwUser = 0;
    waveInHdr.dwFlags = 0;
   
	waveInPrepareHeader(hWaveIn, &waveInHdr, sizeof(WAVEHDR));
		
    waveInAddBuffer(hWaveIn, &waveInHdr, sizeof(WAVEHDR));
	 
	waveInStart(hWaveIn);
	 
	 
	// Wait until finished recording
	//
	while(waveInUnprepareHeader(hWaveIn, &waveInHdr, sizeof(WAVEHDR))
		== WAVERR_STILLPLAYING)
		{
			//w8t
		}
		
	return 0;
}
		