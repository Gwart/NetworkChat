#pragma once
#ifndef _ECHO_TCP_SERVER_H_
#define _ECHO_TCP_SERVER_H_

#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>
#include <iphlpapi.h>

class EchoTCPServer
{
public:
	EchoTCPServer(size_t port, size_t buffSize);
	EchoTCPServer(const EchoTCPServer&) = delete;
	EchoTCPServer& operator=(EchoTCPServer&) = delete;
	virtual ~EchoTCPServer();
	
	int InitTCP();
	virtual void StartTCP();
	
	const SOCKET& GetListenSocket() const;
	const size_t& GetPortTCP() const;
	const size_t& GetBufferTCPSize() const;

protected:
	WSADATA wsaDataTCP;
	struct addrinfo servHintTCP;
	SOCKET listenSocket;
	char* bufferTCP;
	
private:
	fd_set clientSetTCP;
	size_t portTCP;
	size_t bufferTCPSize;
};

#endif