# NetworkChat

*This program is written in C++, for Windows only.

***
## Client

### Dependencies

* gdi32.dll
* kernel32.dll
* msvcrt.dll
* winmm.dll
* ws2_32.dll

_Compilation line_ for **_g++_**: g++ -mwindows -municode winapimain.cpp client.cpp playbackvoice.cpp recordvoice.cpp -owinapimain.exe -lws2_32 -lwinmm

***

## Server

### Dependencies

* kernel32.dll
* msvcrt.dll
* ws2_32.dll

_Compilation line_ for **_g++_**: g++ EchoTCPserver.cpp EchoUDPServer.cpp Server.cpp mainServer.cpp -oserver.exe -lws2_32

***
