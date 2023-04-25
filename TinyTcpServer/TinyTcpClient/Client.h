#ifndef _EasyTcpClient_hpp_
#define _EasyTcpClient_hpp_
#define RECV_BUFF_SZIE 10240

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
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

#include <stdio.h>
#include "DataType.h"

class EasyTcpClient {
private:
	SOCKET _sock;
	bool _isConnect;
	char _szMsgBuf[RECV_BUFF_SZIE];
	int _lastPos = 0;

public:
	EasyTcpClient();

	virtual ~EasyTcpClient();

	void InitSocket();

	int Connect(const char* ip, unsigned short port);

	void Close();

	bool OnRun();

	bool isRun();

	int RecvData(SOCKET cSock);

	virtual void OnNetMsg(DataHeader* header);

	int SendData(DataHeader* header, int nLen);
};

#endif