#ifndef _CLIENT_H_
#define _CLIENT_H_

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include <unistd.h> 
#include <arpa/inet.h>
#include <string.h>

#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#endif

#define BUFF_SIZE 10240
#include "DataType.h"
#include "HighTimer.h"

class Client {
private:
	SOCKET _socket;
	long long _uid;
	char* _buffer;
	int _data_len;
	bool _connect;

public:
	Client();

	~Client();

	bool doInit();

	bool doConnect(const char* ip, unsigned short port);

	bool doSend(Header* header);

	bool doRecv();

	bool doDispose(Header* header);

	bool doRun(timeval time_val);

	bool isRun();

	bool doClose();

	SOCKET Socket(const SOCKET socket = -1);

	long long Uid(long long uid = -1);

	bool Connect(int connect = -1);
};

#endif // !_CLIENT_H_
