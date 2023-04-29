#ifndef Server
#define _Server_h_

#ifdef _WIN32
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

#include "DataType.h"
#include <iostream>
#include <vector>
using namespace std;

class Server {
private:
	SOCKET _socket;
	vector<ClientInfo*> _clients;
	int _times;

public:
	// 无参构造
	Server();

	// 析构函数
	~Server();

	// 初始化SOCKET
	bool initSocket();

	// 绑定端口
	bool doBind(const char* ip, unsigned short port);

	// 监听端口
	bool doListen(int num);

	// 等待客户端
	bool Accept();

	// 发送消息
	bool doSend(SOCKET socket, Header* header);

	// 发送全局消息
	bool doSendAll(Header* header);

	// 接收消息
	bool doRecv(ClientInfo* client);

	// 处理消息
	bool processMsg(SOCKET client_sock, Header* header);

	// 服务器运行
	bool Run(timeval time_val);

	// 运行状态
	bool isRun();

	// 关闭客户端
	bool Close();

	// 查找客户端
	ClientInfo* findClient(SOCKET sock);
	ClientInfo* findClient(long long uid);

};

#endif // !Server
