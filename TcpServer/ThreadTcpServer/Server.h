#ifndef Server
#define _Server_h_

#include "DataType.h"
#include "CoreServer.h"

class Server {
private:
	HighTimer _timer;
	SOCKET _socket;
	std::vector<CoreServer*> core_servers;

public:
	// 无参构造
	Server();

	// 析构函数
	~Server();

	// 初始化SOCKET
	bool doSocket();

	// 绑定端口
	bool doBind(const char* ip, unsigned short port);

	// 监听端口
	bool doListen(int n = 5);

	// 等待客户端
	bool doAccept();

	// 服务器运行
	bool doRun();

	// 开启服务器
	void Start(int n = 1);

	// 运行状态
	bool isRun();

	// 关闭服务器
	bool doClose();

	// 分配客户端
	void assignedClient(ClientInfo* client);

	// 服务器信息
	void timerMsg();
};

#endif // !Server
