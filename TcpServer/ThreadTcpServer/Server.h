#ifndef _Server_h_
#define _Server_h_

#include "DataType.h"
#include "ComputeCore.h"

typedef std::shared_ptr<Client> ClientPtr;
typedef std::shared_ptr<Header> HeaderPtr;
typedef std::shared_ptr<ComputeCore> ComputeCorePtr;
class Server {
private:
	int _core_count;
	char* _ip;
	unsigned short _port;
	SOCKET _socket;
	std::thread _thread;
	std::vector<ComputeCorePtr> _compute_cores;

protected:
	bool initSocket();

	bool doBind();

	bool doListen();

	void doAccept();

	void doSend(ClientPtr& client, HeaderPtr& header);

	int doRecv(ClientPtr& client);

	void processMsg(ClientPtr& client, Header* header);

	void doWork();

	void shareClient(ClientPtr& client);

public:
	Server(const char* ip, unsigned short port, int core);

	~Server();

	bool isRun();
	
	void doClose();

	void doThread();

	size_t clientCount();

	std::pair<int, int> recvCount();

	std::pair<int, int> sendCount();
};

#endif // !_Server_h_
