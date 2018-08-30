#pragma once
#ifndef _SERVER_H_
#define _SERVER_H_

#include "EchoUDPServer.h"
#include "EchoTCPServer.h"
#include <unordered_map>

class Server : public EchoTCPServer, public EchoUDPServer
{
public:
	Server(size_t port, size_t buffUDPSize, size_t buffTCPSize):
		EchoTCPServer(port, buffTCPSize), EchoUDPServer(port, buffUDPSize)
		{ }
	~Server();
	
	void StartUDP() override;
	void StartTCP() override;
	
	void SetClient(const SOCKET& keySock, const sockaddr_in& dest_addr);
	const sockaddr_in& GetClient(const SOCKET& key) const;
	
private:
	void FillCopySet(fd_set&);
	void CheckMessage(const SOCKET&) const;
	void SendToAllTCP(const SOCKET&) const;
	void RemoveClient(SOCKET&);

	std::unordered_map<SOCKET, sockaddr_in> validClients;
};



#endif