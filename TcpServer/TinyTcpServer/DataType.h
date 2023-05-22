#ifndef _DATATYPE_H_
#define _DATATYPE_H_

#ifdef _WIN32
#define FD_SETSIZE 2048
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

#define RECV_BUFF_SIZE 10240
#define SEND_BUFF_SIZE 10240
#define MESSAGE_SIZE 256

#include "HighTimer.h"
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>

class Header;

enum CMD {
	CMD_ERROR,
	CMD_MESSAGE
};

class Client {
public:
	SOCKET _socket;
	short _recv_pos, _send_pos;
	char* _recv_buff, * _send_buff;

	Client(SOCKET socket);

	// ·¢ËÍÏûÏ¢
	void doSend(Header* header);

	virtual ~Client();
};

class Header {
public:
	short _size, _type;

	Header();

	virtual ~Header();
};

class Message : public Header {
public:
	char* msg;

	Message();

	virtual ~Message();
};

#endif // !_DATATYPE_H_
