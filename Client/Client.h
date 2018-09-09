#pragma once
#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <process.h>
#include "recordvoice.h"
#include "playbackvoice.h"

#define DEFAULT_PORT "778"
#define SOUND_BUFFER 1000
#define TEXT_BUFFER 530
#define NICKNAME_BUFFER 30
#define CUSTOM_PORT 14000

class Client
{
	public:
		Client();
		~Client();
		
		const wchar_t* const GetNickname() const;
		
		int InitTCP();
		int Connect(const char*, const wchar_t*);
		const bool& is_connected() const;
		int SendMessageTCP(const wchar_t*);
		const wchar_t* const RecvMessageTCP();
		
		int InitUDP();
		void StartThreadUDP();
		const struct sockaddr_in& GetAddrUDP() const;
		const SOCKET& GetSocketUDP() const;
		
		void Disconnect();
		
		
		
	private:
		static unsigned __stdcall SendVoiceUDP(void*);
		static unsigned __stdcall RecvVoiceUDP(void*);
	
		WSADATA wsaDataUDP;
		SOCKET sockUDP;
		struct sockaddr_in sendAddrUDP;
		HANDLE sendThread;
		HANDLE recvThread;
		
		WSADATA wsaDataTCP;
		SOCKET sockTCP;
		struct addrinfo* result;
		struct addrinfo hints;
		
		wchar_t nickname[NICKNAME_BUFFER];
		wchar_t recvbuf[TEXT_BUFFER];
		bool connection;
		
};


#endif