#ifndef _Server_h_
#define _Server_h_

#include "DataPackage.h"

class Server {
private:
	SOCKET _socket;
	sockaddr_in _addr;
	vector<UserInfo> _clients_info;

public:
	Server();	// 构造函数

	bool InitSocket();	// 初始化socket

	bool toBind();	// 绑定端口

	bool toListen();	// 监听端口

	bool closeSocket();	// 关闭socket

	bool disposeReq();	// 处理请求

	~Server();	// 析构函数
};

#endif