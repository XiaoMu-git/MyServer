#ifndef _Client_h_
#define _Client_h_

#include "DataPackage.h"

class Client {
private:
	SOCKET _socket;
	sockaddr_in _addr;
	UserInfo _info;
	char _recv[4096];

public:
	Client();

	virtual ~Client();

	// 初始化socket
	bool initSocket();

	// 连接服务器
	bool doConnect(const char* ip, int post);

	// 接收消息
	bool doRecv();

	// 发送消息
	bool doSend();

	// 登录服务器
	bool doLogin();

	// 开启服务
	bool doRun(timeval time_val);

	// 处理消息
	bool dispose();

	// 关闭socket
	bool doClose();
};

#endif