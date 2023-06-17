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

#include <functional>
#include <thread>
#include <vector>
#include <atomic>

enum CMD {
	CMD_ERROR,
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT
};

class Header {
public:
	short _type;
	short _size;

	Header() {
		_type = CMD_ERROR;
		_size = sizeof(Header);
	}
};

class UserInfo : public Header {
public:
	char userName[32];
	char passWord[32];

	UserInfo() {
		_size = sizeof(UserInfo);
	}
};

class Response : public Header {
public:
	short _result;
	char _data[128];

	Response() {
		_result = false;
		_size = sizeof(Response);
	}
};

#endif // !_DATATYPE_H_
