#ifndef _CORESERVER_H_
#define _CORESERVER_H_

#include "SendCore.h"

class ComputeCore {
private:
	SOCKET _socket;
	fd_set fd_reads_back;
	SendCore _send_core;
	std::mutex _mutex;
	std::thread* _thread;
	std::vector<Client*> _clients;
	std::vector<Client*> _clients_buffer;

protected:
	// 接收消息
	bool doRecv(Client* client);

	// 处理消息
	void doDispose(Client* client, Header* header);

	// 服务器运行
	void doRun();

	// 运行状态
	bool isRun();

	// 关闭服务器
	void doClose();

public:
	std::atomic_int _recv_num, _recv_pkg;
	
	ComputeCore(SOCKET socket);

	// 开启核心线程
	void doThread();

	// 添加新的客户端
	void addClient(Client* client);

	// 获取客户端数量
	int getClientNum();

	// 获取发送的次数
	int getSendNum();

	// 获取发送包的数量
	int getSendPkg();

	~ComputeCore();
};

#endif // !_CORESERVER_H_
