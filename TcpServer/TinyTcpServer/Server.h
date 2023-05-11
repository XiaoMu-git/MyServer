#ifndef Server
#define _Server_h_

#ifdef _WIN32
#define FD_SETSIZE	1024
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<windows.h>
#include<WinSock2.h>
#include <ctime>
#pragma comment(lib,"ws2_32.lib")
#else
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>

#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#endif

#include "DataType.h"
#include "DataType.h"
#include <iostream>
#include <vector>
using namespace std;

class Server {
private:
	SOCKET _socket;
	vector<ClientInfo*> _clients;
	int _msg_num;
	int _pkg_num;
	int _socket_num;
	time_t _time_1, _time_2;

public:
	// �޲ι���
	Server();

	// ��������
	~Server();

	// ��ʼ��SOCKET
	bool initSocket();

	// �󶨶˿�
	bool doBind(const char* ip, unsigned short port);

	// �����˿�
	bool doListen(int num);

	// �ȴ��ͻ���
	bool Accept();

	// ������Ϣ
	bool doSend(SOCKET socket, Header* header);

	// ����ȫ����Ϣ
	bool doSendAll(Header* header);

	// ������Ϣ
	bool doRecv(ClientInfo* client);

	// ������Ϣ
	bool processMsg(SOCKET client_sock, Header* header);

	// ����������
	bool Run(timeval time_val);

	// ����״̬
	bool isRun();

	// �رտͻ���
	bool Close();

	// ���ҿͻ���
	ClientInfo* findClient(SOCKET sock);
	ClientInfo* findClient(long long uid);

	int pkgNum(int pkg_num = -1);

};

#endif // !Server