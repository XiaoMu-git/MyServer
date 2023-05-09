#ifndef _SERVER_H_
#define _SERVER_H_

#ifdef _WIN32
#define FD_SETSIZE	1024
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<windows.h>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>

#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#endif

#include "DataType.h"
#include "HighTimer.h"
#include <iostream>
#include <vector>
using namespace std;

class Server {
protected:
	SOCKET _socket;
	vector<ClientInfo*> _clients;

public:
	Server();

	~Server();

	bool doInit();

	virtual bool doRun(timeval time_val);

	bool isRun();

	bool doClose();

};

#endif // !_SERVER_H_
