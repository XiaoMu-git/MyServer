#ifndef _CLIENT_H_
#define _CLIENT_H_

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

#define RECV_BUFF_SIZE 10240
#define SEND_BUFF_SIZE 10240

class Client {
public:
	SOCKET _socket;
	long long _uid;
	char* _recv_buff;
	int _recv_data_len;
	char* _send_buff;
	int _send_data_len;

	// 无参构造
	Client();

	// 析构函数
	~Client();

};

#endif // !_CLIENT_H_
