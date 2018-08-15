#include "EchoUDPServer.h"

EchoUDPServer::EchoUDPServer(size_t port, size_t buffSize)
{
	portUDP = port;
	bufferUDPSize = buffSize;
	clientInfoSize = sizeof(clientInfoUDP);
	memset(&clientInfoUDP, 0, clientInfoSize);
	memset(&wsaDataUDP, 0, sizeof(wsaDataUDP));
	memset(&servHintUDP, 0, sizeof(servHintUDP));
	bufferUDP = new char[bufferUDPSize];
	bindedSockUDP = INVALID_SOCKET;
}

EchoUDPServer::~EchoUDPServer()
{
	delete[] bufferUDP;
	closesocket(bindedSockUDP);
	WSACleanup();
}

int EchoUDPServer::InitUDP()
{
	if(WSAStartup(0x0202, &wsaDataUDP))
	{
		std::cerr << "WSAStartup ERROR\n";
		return -1;
	}
	
	if((bindedSockUDP = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) 
		== INVALID_SOCKET)
	{
		std::cerr << "socket func error";
		WSACleanup();
		return -1;
	}
	
	servHintUDP.sin_addr.S_un.S_addr = INADDR_ANY;
	servHintUDP.sin_family = AF_INET;
	servHintUDP.sin_port = htons(portUDP);
	
	if(bind(bindedSockUDP, (sockaddr*)&servHintUDP, sizeof(servHintUDP))
		== SOCKET_ERROR)
	{
		std::cerr << "bind ERROR";
		closesocket(bindedSockUDP);
		WSACleanup();
		return -1;
	}
	
	return 0;
}

void EchoUDPServer::StartUDP()
{
	std::cerr << "Waiting for connections \n";
	
	
	while(true)
	{
		//recv a DGRAM from a client
		if(recvfrom(bindedSockUDP, bufferUDP, bufferUDPSize, 0, 
			(SOCKADDR* ) &clientInfoUDP, &clientInfoSize) == SOCKET_ERROR)
		{
			std::cerr << "recvfrom Error" << WSAGetLastError() << std::endl;
		}
		
		//send the DGRAM to the client
		if(sendto(bindedSockUDP, bufferUDP, bufferUDPSize, 0, 
			(SOCKADDR* ) &clientInfoUDP, clientInfoSize) == SOCKET_ERROR)
		{
			std::cerr << "sendto Error" << WSAGetLastError() << std::endl;	
		}
	}
}

const SOCKET& EchoUDPServer::GetBindedSockUDP() const
{
	return bindedSockUDP;
}

const size_t& EchoUDPServer::GetPortUDP() const
{
	return portUDP;
}
	
const size_t& EchoUDPServer::GetBufferUDPSize() const
{
	return bufferUDPSize;
}
	
const WSAData& EchoUDPServer::GetWsaDataUDP() const
{
	return wsaDataUDP;
}


	
	
	
	
	