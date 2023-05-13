#ifndef _DATATYPE_H_
#define _DATATYPE_H_

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

#define MESSAGE_SIZE 1008
#define BUFF_SIZE 10240

#include "HighTimer.h"
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>

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
	char* _message;
	Message() {
		_message = new char[MESSAGE_SIZE];
		_type = CMD_MESSAGE;
		_length = sizeof(Message);
		_uid = -1;
	}
	~Message() { delete _message; };
};

class ClientInfo {
public:
	SOCKET _socket;
	long long _uid;
	char* _buffer;
	int _data_len;
	ClientInfo() {
		_buffer = new char[BUFF_SIZE];
		_socket = INVALID_SOCKET;
		_data_len = 0;
		_uid = -1;
	}
};

#endif // !_DATATYPE_H_
