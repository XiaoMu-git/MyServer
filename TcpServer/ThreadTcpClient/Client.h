#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "DataType.h"
#include "HighTimer.h"

class Client {
private:
	SOCKET _socket;
	char* _recv_buff;
	char* _send_buff;
	int _recv_pos, _send_pos;
	bool _connect;

public:
	Client();

	// 初始化 socket
	bool initSocket();

	// 连接服务器
	bool doConnect(const char* ip, unsigned short port);

	// 发送消息
	void doSend(Header* header);

	// 接收消息
	int doRecv();
	
	// 处理消息
	void doDispose(Header* header);

	// 运行函数
	void doRun();

	// 运行状态
	bool isRun();

	// 关闭客户端
	void doClose();

	// _socket 接口
	SOCKET Socket(const SOCKET socket = -1);

	// _connect 接口
	bool Connect(int connect = -1);

	~Client();
};

#endif // !_CLIENT_H_
