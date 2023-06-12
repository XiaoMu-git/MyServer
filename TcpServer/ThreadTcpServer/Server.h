#ifndef _Server_h_
#define _Server_h_

#include "DataType.h"

typedef std::shared_ptr<Client> ClientPtr;
typedef std::shared_ptr<Header> HeaderPtr;
class Server {
private:
	char* _ip;
	unsigned short _port;
	SOCKET _socket;
	std::thread _thread;
	std::vector<ClientPtr> _clients;

protected:
	bool initSocket();

	bool doBind();

	bool doListen();

	void doAccept();

	void doSend(ClientPtr& client, HeaderPtr& header);

	int doRecv(ClientPtr& client);

	void processMsg(ClientPtr& client, Header* header);

	void doWork();

public:
	std::atomic_int _recv_count, _recv_pkg;
	std::atomic_int _send_count, _send_pkg;

	Server(const char* ip, unsigned short port);

	~Server();

	bool isRun();
	
	void doClose();

	void doThread();

	size_t clientCount();
};

#endif // !_Server_h_
