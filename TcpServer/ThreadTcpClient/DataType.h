#ifndef _DATATYPE_H_
#define _DATATYPE_H_

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

#define RECV_BUFF_SIZE 10240
#define SEND_BUFF_SIZE 10240
#define MESSAGE_SIZE 256

enum CMD {
	CMD_ERROR,
	CMD_MESSAGE,
	CMD_NULL
};

class Header {
public:
	short _size, _type;

	Header() {
		_type = CMD_ERROR;
		_size = sizeof(Header);
	}

	virtual ~Header() {}
};

class Message : public Header {
public:
	char* msg;

	Message() {
		_type = CMD_MESSAGE;
		msg = new char[MESSAGE_SIZE];
		_size = sizeof(Message) + MESSAGE_SIZE;
	}

	virtual ~Message() {
		delete msg;
	}
};

#endif // !_DATATYPE_H_
