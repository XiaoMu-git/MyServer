#ifndef Server
#define _Server_h_

#include "DataType.h"
#include "ComputeCore.h"
#include "HighTimer.h"

class Server {
private:
	const char* _ip;
	unsigned short _port;
	SOCKET _socket;
	HighTimer _timer;
	std::vector<ComputeCore*> _compute_core;

protected:
	// 初始化 socket
	bool initSocket();

	// 绑定端口
	bool doBind();

	// 监听端口
	bool doListen(int num);

	// 等待客户端
	void waitAccept();

	// 服务器运行
	void doRun();

	// 运行状态
	bool isRun();

	// 关闭服务器
	bool doClose();

	// 分配客户端
	void assignedClient(Client* client);

	// 处理服务器消息
	void timeMsg();

public:
	Server(const char* ip, unsigned short port);

	// 开启服务器
	void Start(int n = 1);

	~Server();
};

#endif // !Server
