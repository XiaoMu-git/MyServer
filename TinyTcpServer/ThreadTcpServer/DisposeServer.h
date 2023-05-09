#ifndef _DISPOSESERVER_H_
#include "Server.h"

class DisposeServer : public Server {
private:
	vector<ClientInfo*> _clients;
	char* _buffer;
	int _buffer_len;

public:
	DisposeServer();

	~DisposeServer();

	bool doSend(SOCKET socket, Header* header);

	bool doRun(timeval time_val) override;

	bool doRecv(ClientInfo* client);

	bool doDispose(SOCKET client_sock, Header* header);

};

#endif // !_DISPOSESERVER_H_
