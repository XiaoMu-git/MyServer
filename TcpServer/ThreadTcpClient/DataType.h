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

enum CMD {
	CMD_ERROR,
	CMD_MESSAGE,
	CMD_NULL
};

class Header {
public:
	short _type;
	short _length;
	long long _uid;
	Header() {
		_length = sizeof(Header);
		_type = CMD_NULL;
		long long _uid = -1;
	}
};

class Message : public Header {
public:
	char _message[256];
	Message() {
		_type = CMD_MESSAGE;
		_length = sizeof(Message);
		_uid = -1;
	}
};

#endif // !_DATATYPE_H_
