#ifndef _DISPOSESERVER_H_
#include "Server.h"
#include <functional>

class DisposeServer/* : public Server */{
private:
	SOCKET _socket;
	vector<ClientInfo*> _clients;
	vector<ClientInfo*> _clients_buffer;
	char* _buffer;
	int _buffer_len;
	mutex _mutex;
	thread* _thread;

public:
	DisposeServer();

	~DisposeServer();

	bool doInit();

	bool doRun(timeval time_val);

	bool isRun();

	bool doSend(SOCKET socket, Header* header);

	bool doRecv(ClientInfo* client);

	bool doDispose(SOCKET client_sock, Header* header);

	bool doClose();

	void doStart();

	int getClientsBufferLen();

	void addClientBuffer(ClientInfo* client);

};

#endif // !_DISPOSESERVER_H_
