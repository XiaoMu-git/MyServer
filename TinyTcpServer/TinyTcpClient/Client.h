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

public:
	// �޲ι���
	Client();

	// ��������
	~Client();

	// ��ʼ��SOCKET
	bool initSocket();

	// ���ӷ�����
	bool doConnect(const char* ip, unsigned short port);

	// ������Ϣ
	bool doSend(Header* header);

	// ������Ϣ
	bool doRecv();

	// ������Ϣ
	bool processMsg(Header* header);

	// �ͻ�������
	bool Run(const timeval time_val);

	// ����״̬
	bool isRun();

	// �رտͻ���
	bool Close();

	SOCKET getSocket();

	char* getUsername();

	void setUsername(const char* username);

	char* getPassword();

	void setPassword(const char* password);

};

#endif // !Client
