#ifndef _DataPackage_h_
#define _DataPackage_h_

#ifdef _WIN32
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#include <WinSock2.h>
	#pragma comment(lib, "ws2_32.lib")
#else
	#include <unistd.h>
	#include <arpa/inet.h>
	#include <string.h>

	#define SOCKET int
	#define INVALID_SOCKET	(SOCKET)(~0)
	#define SOCKET_ERROR			(-1)
#endif

#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

enum CMD {
	CMD_ERROR,
	CMD_TREU,
	CMD_FALSE,
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_RESPONSE,
	CMD_MESSAGE,
	CMD_USERINFO
};

class Header {
public:
	short length;
	short cmd_type;
	Header() {
		this->length = sizeof(Header);
		this->cmd_type = -1;
	}
};

class Response : public Header {
public:
	short resp;
	Response() {
		this->length = sizeof(Response);
		this->cmd_type = CMD_RESPONSE;
		this->resp = CMD_TREU;
	}
};

class Message : public Header {
public:
	char msg[128];
	char username[32];
	SOCKET send_socket;
	Message() {
		this->length = sizeof(Message);
		this->cmd_type = CMD_MESSAGE;
	}
};

class UserInfo : public Header {
public:
	char username[32];
	char password[32];
	SOCKET socket;
	UserInfo() {
		this->length = sizeof(UserInfo);
	}
};






#endif