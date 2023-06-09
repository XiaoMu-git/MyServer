#ifndef _ComputeCore_h_
#define _ComputeCore_h_

#include "DataType.h"
#include "TaskCore.h"

class ComputeCore {
private:
	bool _is_change;
	SOCKET _socket;
	TaskCore _task_core;
	std::mutex _mutex;
	std::thread _thread;
	std::vector<ClientPtr> _clients, _clients_buff;

protected:
	void doSend(ClientPtr& client, HeaderPtr& header);

	int doRecv(ClientPtr& client);

	void processMsg(ClientPtr& client, Header* header);

	void doWork();

	void checkTime();

public:
	std::atomic_int _recv_count, _recv_pkg;
	std::atomic_int _send_count, _send_pkg;
	ComputeCore(SOCKET _socket);

	~ComputeCore();

	void doThread();

	void addClient(ClientPtr& client);

	bool isRun();

	void doClose();

	size_t clientCount();
};

#endif // !_ComputeCore_h_
