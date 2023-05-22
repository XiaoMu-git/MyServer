#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "DataType.h"

class Client {
private:
	const char* _ip;
	unsigned short _port;
	char* _recv_buff;
	char* _send_buff;
	int _recv_pos, _send_pos;

protected:
	// 初始化socket
	bool initSocket();

	// 连接服务器
	bool doConnect();

	// 接收消息
	int doRecv();

	// 发送消息
	void doSend(Header* header);

	// 处理消息
	void doDispose(Header* header);

	// 运行状态
	bool isRun();

	// 关闭客户端
	void doClose();

public:
	SOCKET _socket;
	std::atomic_int _recv_num, _recv_pkg;
	std::atomic_int _send_num, _send_pkg;

	Client(const char* ip, unsigned short port);

	// 运行函数
	bool doRun(Header* header);

	// 启动客户端
	void Start();

	virtual ~Client();
};

#endif // !_CLIENT_H_
