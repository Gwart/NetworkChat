#include "Server.h"

#define CUSTOM_SRC_PORT 14000

Server::~Server()
{
	for(auto& it : validClients)
	{
		shutdown(it.first, SD_BOTH);
		closesocket(it.first);
	}
}
 
void Server::StartUDP()
{
	sockaddr_in info;
	int infoLen = sizeof(info);
	memset(&info, 0, sizeof(info));
	
	while(true)
	{
		//recv a DGRAM from a client
		if(recvfrom(bindedSockUDP, bufferUDP, GetBufferUDPSize(), 0, 
			(sockaddr* ) &info, &infoLen) == SOCKET_ERROR)
		{
			std::cerr << "recvfrom Error" << WSAGetLastError() << std::endl;
		}
		
		//send the DGRAM to the all connected clients
		for(const auto& it : validClients)
		{
			if((it.second).sin_addr.s_addr != info.sin_addr.s_addr)
			{
				if(sendto(GetBindedSockUDP(), bufferUDP, GetBufferUDPSize(),
					0, (sockaddr*) &(it.second), clientInfoSize) 
						== SOCKET_ERROR)
				{
					std::cerr << "sendto Error" << 
						WSAGetLastError() << std::endl;	
				}
			}
		}
	}
	
}

void Server::StartTCP()
{
	fd_set copySet;
	size_t socketCount = 0;
	
	std::cout << "Waiting for connections\n";
	
	while(true)
	{
		FillCopySet(copySet);
		
		if((socketCount = select(0, &copySet, NULL, NULL, NULL)) 
			== SOCKET_ERROR)
		{
			std::cerr << "select ERROR" << std::endl;
		}
		
		for(int i = 0; i < socketCount; ++i)
		{
			if(copySet.fd_array[i] == listenSocket)
			{
				//Accept a socket and add it to the unordered_map
				SOCKET client = accept(listenSocket,
									   (sockaddr*) &clientInfoUDP,
									   &clientInfoSize);
				//Changing port in the clientInfoUDP structure
				//cause we need a custom(binded from a client) port
				//for the UDP sendto function(from client code)
				//PS we can use a default random port
				//from the client sendto function, but we must to avoid
				//additional code in the StartUDPs cycle
				clientInfoUDP.sin_port = htons(CUSTOM_SRC_PORT);
				
				SetClient(client, (sockaddr_in)clientInfoUDP);
				
				std::cerr << "\nNew client connected. There is " 
					<< validClients.size() << " on the server.\n";
			}
			else 
			{
				//recv a messsage
				int result = recv(copySet.fd_array[i],
								  bufferTCP, 
								  GetBufferTCPSize(), 
								  0);
				if(result == SOCKET_ERROR)
				{
					std::cerr << "recv Error " << WSAGetLastError() << '\n';
				}
				//if the connection was gracefully closed
				else if(result == 0)
				{
					RemoveClient(copySet.fd_array[i]);
					std::cerr << "\nA client disconnected. There is " 
						<< validClients.size() << " on the server.\n";
				}
				else
				{
				//send a message to all clients
					SendToAllTCP(copySet.fd_array[i]);
				}
			}
		}
	}
}

void Server::SetClient(const SOCKET& keySock, const sockaddr_in& destAddr)
{
	validClients[keySock] = destAddr;
}

const sockaddr_in& Server::GetClient(const SOCKET& key) const
{
	const auto& it = validClients.find(key);
	
	if(it != validClients.end())
		return it->second; 
}

void Server::FillCopySet(fd_set& copySet)
{
	FD_ZERO(&copySet);
	FD_SET(listenSocket, &copySet);
	for(const auto& it : validClients)
		FD_SET(it.first, &copySet);
}

void Server::SendToAllTCP(const SOCKET& msgOwner) const
{
	for(const auto& it : validClients)
	{
		if(it.first != msgOwner)
		{
			if(send(it.first, bufferTCP, GetBufferTCPSize(), 0) 
				== SOCKET_ERROR)
			{
				std::cerr << "send Error " << WSAGetLastError() << std::endl;
			}
		}
	}
}

void Server::RemoveClient(SOCKET& client)
{
	 if(validClients.erase(client) == 0)
	 {
		 std::cerr << "RemoveClient error(client is not in the map)" 
			<< std::endl;
	 }
	 shutdown(client, SD_BOTH);
	 closesocket(client);
}







