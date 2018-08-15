#include "Server.h"
#include <process.h>

unsigned __stdcall UDPThread(void*);

int main()
{
	Server mainServer(778, 1000, 1060);
	
	_beginthreadex(NULL, 0, UDPThread, &mainServer, 0, NULL);
	
	mainServer.InitTCP();
	mainServer.StartTCP();
	
	
	return 0;
}

unsigned __stdcall UDPThread(void* object)
{
	Server* thisServer = (Server*) object;
	
	thisServer->InitUDP();
	thisServer->StartUDP();
	
}
