#pragma once
#ifndef _ECHO_UDP_SERVER_H_
#define _ECHO_UDP_SERVER_H_

#include <iostream>
#include <winsock2.h>

class EchoUDPServer
{
public:
	EchoUDPServer(size_t port, size_t buffSize);
	~EchoUDPServer();
	EchoUDPServer(const EchoUDPServer&) = delete;
	EchoUDPServer& operator=(EchoUDPServer&) = delete;
	
	int InitUDP();
	virtual void StartUDP();
	
	const SOCKET& GetBindedSockUDP() const;
	const size_t& GetPortUDP() const;
	const size_t& GetBufferUDPSize() const;
	const WSAData& GetWsaDataUDP() const;
	
protected:	
	struct sockaddr_in clientInfoUDP;
	int clientInfoSize;
	SOCKET bindedSockUDP;
	char* bufferUDP;
	
private:
	size_t portUDP;
	size_t bufferUDPSize;
	WSAData wsaDataUDP;
	sockaddr_in servHintUDP;
};


#endif