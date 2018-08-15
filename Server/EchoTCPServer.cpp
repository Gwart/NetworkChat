#include "EchoTCPServer.h"


EchoTCPServer::EchoTCPServer(size_t port, size_t buffSize)
{
	bufferTCP = new char[buffSize];
	FD_ZERO(&clientSetTCP);
	bufferTCPSize = buffSize;
	portTCP = port;
	listenSocket = INVALID_SOCKET;
	memset(&wsaDataTCP, 0, sizeof(WSADATA));
	memset(&servHintTCP, 0, sizeof(struct addrinfo));
}

EchoTCPServer::~EchoTCPServer()
{
	delete[] bufferTCP;
	for(int i = 0; i < clientSetTCP.fd_count; ++i)
	{
		shutdown(clientSetTCP.fd_array[i], SD_BOTH);
		closesocket(clientSetTCP.fd_array[i]);
	}
	WSACleanup();
	freeaddrinfo(&servHintTCP);
}

int EchoTCPServer::InitTCP()
{
	struct addrinfo* result = NULL;
	char portBuf[10];
	itoa(portTCP, portBuf, 10);
	
	if(WSAStartup(0x0202, &wsaDataTCP))
	{
		std::cerr << "WSAStartup ERROR";
		return -1;
	}
	
	servHintTCP.ai_family   = AF_INET;
	servHintTCP.ai_socktype = SOCK_STREAM;
	servHintTCP.ai_protocol = IPPROTO_TCP;
	servHintTCP.ai_flags	  = AI_PASSIVE;
	
	if(getaddrinfo(NULL, portBuf, &servHintTCP, &result))
	{
		std::cerr << "getaddrinfo ERROR";
		WSACleanup();
		return -1;
	}
	
	if(INVALID_SOCKET == ( listenSocket = 
	socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
	{
		std::cerr << "socket func error";
		WSACleanup();
		return -1;
	}
	
	if(bind(listenSocket, result->ai_addr, (int)result->ai_addrlen))
	{
		std::cerr << "bind ERROR";
		closesocket(listenSocket);
		WSACleanup();
		return -1;
	}
	
	if(listen(listenSocket, SOMAXCONN))
	{
		std::cerr << "listen ERROR";
		WSACleanup();
		closesocket(listenSocket);
		return -1;
	}
	
	freeaddrinfo(result);
	
	return 0;
}

void EchoTCPServer::StartTCP()
{
	fd_set copySet;
	FD_SET(listenSocket, &clientSetTCP);
	size_t socketCount = 0;
	
	std::cout << "Waiting for connections\n";
	
	while(true)
	{
		copySet = clientSetTCP;
		
		if((socketCount = select(0, &copySet, NULL, NULL, NULL)) 
			== SOCKET_ERROR)
		{
			std::cerr << "select ERROR" << std::endl;
		}
		
		for(int i = 0; i < socketCount; ++i)
		{
			if(copySet.fd_array[i] == listenSocket)
			{
				//Accept a socket and add it to the clientSetTCP
				SOCKET client = accept(listenSocket, NULL, NULL);
				FD_SET(client, &clientSetTCP);
				std::cerr << "\nNew client connected. There is " 
					<< clientSetTCP.fd_count - 1 << " on the server.\n";
			}
			else 
			{
				//recv a messsage
				int result = recv(copySet.fd_array[i],
								  bufferTCP, 
								  bufferTCPSize, 
								  0);
				if(result == SOCKET_ERROR)
				{
					std::cerr << "recv Error " << WSAGetLastError() << '\n';
				}
				//if the connection was gracefully closed
				else if(result == 0)
				{
					closesocket(copySet.fd_array[i]);
					FD_CLR(copySet.fd_array[i], &clientSetTCP);
					std::cerr << "\nA client disconnected. There is " 
					<< clientSetTCP.fd_count - 1 << " on the server.\n";
				}
				else
				{
				//send a message to the client
					if(send(copySet.fd_array[i],
						bufferTCP, bufferTCPSize, 0) == SOCKET_ERROR)
					{
						std::cerr << "send Error " <<
							WSAGetLastError() << std::endl;
					}
					
				}
			}
		}
	}
}

const SOCKET& EchoTCPServer::GetListenSocket() const
{
	return listenSocket;
}

const size_t& EchoTCPServer::GetPortTCP() const
{
	return portTCP;
}

const size_t& EchoTCPServer::GetBufferTCPSize() const
{
	return bufferTCPSize;
}









