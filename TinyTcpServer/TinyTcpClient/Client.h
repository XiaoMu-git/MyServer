#ifndef _Client_h_
#define _Client_h_

#include "DataPackage.h"

class Client {
private:
	SOCKET _socket;
	sockaddr_in _sockaddr;

public:
	Client();

	virtual ~Client();

	// 初始化socket
	bool initSocket();

	// 连接服务器
	bool doConnect(const char* ip, int post);

	// 关闭socket
	bool doClose();

	// 
};

#endif