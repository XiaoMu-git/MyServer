#ifndef _CONNECTSERVER_H_
#define _CONNECTSERVER_H_
#define DISPOSESERVER_NUM 4
#include "Server.h"
#include "DisposeServer.h"

class ConnectServer : public Server {
private:
	DisposeServer* _postServer[DISPOSESERVER_NUM];
	vector<ClientInfo*> _clients_buffer[DISPOSESERVER_NUM];

public:
	ConnectServer();

	~ConnectServer();

	bool Accept();

	bool doRun(timeval time_val) override;

};

#endif // !_CONNECTSERVER_H_
