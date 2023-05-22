#ifndef _SERVER_H_
#define _SERVER_H_

#include "DataType.h"
#include "HighTimer.h"

class Server {
private:
	const char* _ip;
	unsigned short _port;
	SOCKET _socket;
	HighTimer _timer;
	std::mutex _mutex;
	std::map<SOCKET, Client*> _clients;

protected:
	// 初始化 socket
	bool initSocket();

	// 绑定端口
	bool doBind();

	// 监听端口
	bool doListen(int num);

	// 等待客户端连接
	void waitAccept();

	// 接收消息
	int doRecv(Client* client);

	// 发送消息
	void doSend(Client* client, Header* header);

	// 处理消息
	void doDispose(Client* client, Header* header);

	// 运行函数
	void doRun();

	// 关闭服务器
	void doClose();

	// 运行状态
	bool isRun();

	// 服务器信息
	void printInfo();

public:
	std::atomic_int _recv_num, _recv_pkg;
	std::atomic_int _send_num, _send_pkg;

	Server(const char* ip, unsigned short port);

	// 开启服务器
	void Start();

	virtual ~Server();
};

#endif // !_SERVER_H_
