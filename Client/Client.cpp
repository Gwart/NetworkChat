#include "client.h"

Client::Client()
{
	memset(&wsaDataUDP, 0, sizeof(WSADATA));
	sockUDP = INVALID_SOCKET;
	memset(&sendAddrUDP, 0, sizeof(sockaddr_in));
	sendThread = NULL;
	recvThread = NULL;
	
	memset(&wsaDataTCP, 0, sizeof(WSADATA));
	sockTCP = INVALID_SOCKET;
	result = NULL;
	memset(&hints, 0, sizeof(addrinfo));
	
	connection = false;
	
}

Client::~Client()
{
	Disconnect();
	CloseHandle(sendThread);
	CloseHandle(recvThread);
	WSACleanup();
}

const wchar_t* const Client::GetNickname() const
{
	return nickname;
}

int Client::InitTCP()
{
	if(WSAStartup(MAKEWORD(2, 2), &wsaDataTCP))
	{
		MessageBoxW(NULL, L"WSAStartup TCP Error", L"ERROR", MB_OK);
		return -1;
	}
	
	if((sockTCP = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == 
		INVALID_SOCKET) 
		
	{	
		MessageBoxW(NULL, L"socket TCP Error", L"ERROR", MB_OK);
		return -1;
	}	
	
	return 0;
}

int Client::Connect(const char* addr, const wchar_t* nick)
{
	if(connection == true)
	{
		Disconnect();
		InitTCP();
		InitUDP();
	}
	else 
	{
		InitTCP();
		InitUDP();
	}
	
	for(int i = 0; i < wcslen(nick); ++i)
	{
		nickname[i] = nick[i];
	}
	
	nickname[wcslen(nick)] = L':';
	nickname[wcslen(nick) + 1] = L'\0';
	
	
	memset(&sendAddrUDP, 0, sizeof(sendAddrUDP));
	
	int iPort;
	iPort = atoi(DEFAULT_PORT);
	
	sendAddrUDP.sin_family = AF_INET;
	sendAddrUDP.sin_port = htons(iPort);
	sendAddrUDP.sin_addr.s_addr = inet_addr(addr);
	
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	
	if(getaddrinfo(addr, DEFAULT_PORT, &hints, &result)) //1
	{
		MessageBoxW(NULL, L"getaddrinfo Error", L"ERROR", MB_OK);
		Disconnect();
		return -1;
	}
	//Think about a timer here, because if an address does exist,
	//and there is no server application on the remote address
	//the connect function suspends your programm 
	if(connect(sockTCP,result->ai_addr, (int)result->ai_addrlen))
	{
		MessageBoxW(NULL, L"connect Error", L"ERROR", MB_OK);
		Disconnect();
		return -1;
	}
	
	if(connect(sockUDP, (sockaddr* ) &sendAddrUDP, sizeof(sendAddrUDP)))
	{
		MessageBoxW(NULL, L"connect Error", L"ERROR", MB_OK);
		Disconnect();
		return -1;
	}
	
	//Set nonblocking TCP socket
	unsigned long nb = 1;
	if(ioctlsocket(sockTCP, FIONBIO, (unsigned long* )&nb) == SOCKET_ERROR)
	{
		MessageBoxW(NULL, L"Setting nonblock TCP sock error", L"ERROR", MB_OK);
		return -1;
	}
	
	
	freeaddrinfo(result);
	
	if (sockTCP == INVALID_SOCKET) 
	{
        MessageBoxW(NULL, L"Unable to connect to the server", L"ERROR", MB_OK);
        return -1;
    }
	
	connection = true;
	
	return 0;
}

const bool& Client::is_connected() const
{
	return connection;
}

int Client::SendMessageTCP(const wchar_t* msg)
{
	
	if(sockTCP == INVALID_SOCKET)
	{
		return -1;
	}
	wchar_t sendbuf[TEXT_BUFFER];
	wcscat(sendbuf, nickname);
	wcscat(sendbuf, msg);
	
	if(send(sockTCP, (char*)sendbuf, sizeof(sendbuf), 0) == SOCKET_ERROR)
		return -1;
	
	return 0;
}

const wchar_t* const Client::RecvMessageTCP()	
{

	if(recv(sockTCP, (char*)recvbuf, sizeof(recvbuf), 0) > 0)
	{	
		wcscat((wchar_t*)recvbuf, L"\r\n");
		return (wchar_t*)recvbuf;
	}
	return NULL;
}

int Client::InitUDP()
{
	if(WSAStartup(MAKEWORD(2, 2), &wsaDataUDP))
	{
		MessageBoxW(NULL, L"WSAStartup UDP Error", L"ERROR", MB_OK);
		return -1;
	}
	
	if((sockUDP = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
	{
		MessageBoxW(NULL, L"socket UDP Error", L"ERROR", MB_OK);
		return -1;
	}
	
	//bind our cusom port (need this for server optimizatoin, maybe temporary)
	sockaddr_in local;
	memset(&local, 0, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons(CUSTOM_PORT);
	
	if(bind(sockUDP, (sockaddr* ) & local, sizeof(local)) == SOCKET_ERROR)
	{
		MessageBoxW(NULL, L"CUSTOM_PORT bind error", L"ERROR", MB_OK);
		return -1;
	}
	
	return 0;
}

void Client::StartThreadUDP()
{
	sendThread = (HANDLE)_beginthreadex(NULL, 
									   0, 
									   SendVoiceUDP,                       
									   this, 
									   0, 
									   NULL);
									   
	recvThread = (HANDLE)_beginthreadex(NULL, 
									   0, 
									   RecvVoiceUDP,                       
									   this, 
									   0, 
									   NULL);
}

const struct sockaddr_in& Client::GetAddrUDP() const
{
	return sendAddrUDP;
}

const SOCKET& Client::GetSocketUDP() const
{
	return sockUDP;
}

void Client::Disconnect()
{		
	shutdown(sockTCP, SD_BOTH);
	shutdown(sockUDP, SD_BOTH);
	closesocket(sockTCP);
	closesocket(sockUDP);
	
	sockTCP = INVALID_SOCKET;
	sockUDP = INVALID_SOCKET;
	result = NULL;
	connection = false;
	Sleep(1000);
}

unsigned __stdcall Client::SendVoiceUDP(void* object)
{
	Client* thisClient = (Client*) object;
	RecordVoice clientRV;
	char sendbuf[SOUND_BUFFER];
	
	while(thisClient->is_connected())
	{
			clientRV.Record(sendbuf, sizeof(sendbuf)); 
			
			send(thisClient->GetSocketUDP(), sendbuf, sizeof(sendbuf), 0);
	}
	
}

unsigned __stdcall Client::RecvVoiceUDP(void* object)
{
	Client* thisClient = (Client*) object;
	PlaybackVoice clientPV;
	char recvbuf[SOUND_BUFFER];
	SOCKET sockUDP = thisClient->GetSocketUDP(); 
	struct sockaddr_in sendAddr = thisClient->GetAddrUDP();
	
	int len = sizeof(sockaddr_in);
	
	while(thisClient->is_connected())
	{
		if(recv(sockUDP, recvbuf, sizeof(recvbuf),0) 
			== SOCKET_ERROR)
			continue;
		
		clientPV.Play(recvbuf, sizeof(recvbuf));
	}
	
}



