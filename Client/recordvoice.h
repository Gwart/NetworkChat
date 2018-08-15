#pragma once
#ifndef _RECORDINGVOICE_H_
#define _RECORDINGVOICE_H_
#include <winsock2.h>
#include <mmsystem.h>


class RecordVoice
{
	public:
		RecordVoice();
		~RecordVoice();
		
		int Record(char* buf, size_t bufferSize);
		
	
	private:
		HWAVEIN hWaveIn;
		WAVEHDR waveInHdr;
		WAVEFORMATEX pFormat;
		unsigned short int samplesPs = 8000;
};
					
#endif