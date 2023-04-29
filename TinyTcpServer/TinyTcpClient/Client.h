#ifndef _Client_h_
#define _Client_h_

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

#include "DataType.h"
#include <iostream>
using namespace std;

class Client {
private:
	SOCKET _socket;
	char _username[32];
	char _password[32];
	long long _uid;
	int _data_len;
	int _times;

public:
	// 无参构造
	Client();

	// 析构函数
	~Client();

	// 初始化SOCKET
	bool initSocket();

	// 连接服务器
	bool doConnect(const char* ip, unsigned short port);

	// 发送消息
	bool doSend(Header* header);

	// 接收消息
	bool doRecv();

	// 处理消息
	bool processMsg(Header* header);

	// 客户端运行
	bool Run(timeval time_val);

	// 运行状态
	bool isRun();

	// 关闭客户端
	bool Close();

	SOCKET Socket(const SOCKET socket = -1);

	char* Username(const char* username = NULL);

	char* Password(const char* password = NULL);

	long long Uid(long long uid = -1);
};

#endif // !Client
