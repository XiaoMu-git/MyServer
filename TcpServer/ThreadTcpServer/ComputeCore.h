#ifndef _CORESERVER_H_
#define _CORESERVER_H_

#include "DataType.h"
#include "Client.h"
#include <functional>
#include <mutex>
#include <vector>
#include <atomic>

class ComputeCore {
private:
	int _id;
	fd_set fd_reads_back;
	SOCKET _socket;
	std::mutex _mutex;
	std::thread _thread;
	std::vector<Client*> _clients;
	std::vector<Client*> _clients_buffer;

public:
	std::atomic_int _recv_package;
	std::atomic_int _recv_count;

	// 有参构造
	ComputeCore(int id, SOCKET socket = INVALID_SOCKET);

	// 析构函数
	~ComputeCore();

	// 发送消息
	bool doSend(SOCKET socket, Header* header);

	// 接收消息
	bool doRecv(Client* client);

	// 处理消息
	bool doDispose(SOCKET client_sock, Header* header);

	// 服务器运行
	bool doRun();

	// 开启服务器线程
	void Start();

	// 运行状态
	bool isRun();

	// 关闭服务器
	bool doClose();

	// 添加新的客户端
	void addClient(Client* client);

	// 获取客户端数量
	int getClientNum();

	// 获取线程 id
	int getId();

};

#endif // !_CORESERVER_H_
