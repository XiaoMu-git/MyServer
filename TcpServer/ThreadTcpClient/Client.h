#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "DataType.h"
#include "HighTimer.h"

class Client {
private:
	int _recv_pos, _send_pos;
	char* _recv_buff, * _send_buff;
	char _ip[10];
	unsigned short _port;
	SOCKET _socket;
	std::thread* _thread;

	// 初始化 socket
	bool initSocket();

	// 连接服务器
	bool doConnect();

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

public:
	int _recv_count, _recv_pkg;
	int _send_count, _send_pkg;

	Client(const char ip[], unsigned short port);

	// 启动线程
	void doThread();

	~Client();
};

#endif // !_CLIENT_H_
