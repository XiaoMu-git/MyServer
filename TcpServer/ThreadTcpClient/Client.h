#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "DataType.h"
#include "HighTimer.h"

class Client {
private:
	SOCKET _socket;
	long long _uid;
	char* _recv_buff;
	int _recv_data_len;
	char* _send_buff;
	int _send_data_len;
	bool _connect;

public:
	Client();

	~Client();

	bool doSocket();

	bool doConnect(const char* ip, unsigned short port);

	bool doSend(Header* header);

	bool doRecv();

	bool doDispose(Header* header);

	bool doRun(timeval time_val);

	bool isRun();

	bool doClose();

	SOCKET Socket(const SOCKET socket = -1);

	long long Uid(long long uid = -1);

	bool Connect(int connect = -1);
};

#endif // !_CLIENT_H_
