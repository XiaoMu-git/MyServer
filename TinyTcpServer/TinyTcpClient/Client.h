#ifndef _Client_h_
#define _Client_h_

#include "DataPackage.h"

class Client {
private:
	SOCKET _socket;
	sockaddr_in _addr;
	UserInfo _info;
	char _recv[4096];

public:
	Client();

	virtual ~Client();

	// ��ʼ��socket
	bool initSocket();

	// ���ӷ�����
	bool doConnect(const char* ip, int post);

	// ������Ϣ
	bool doRecv();

	// ������Ϣ
	bool doSend();

	// ��¼������
	bool doLogin();

	// ��������
	bool doRun(timeval time_val);

	// ������Ϣ
	bool dispose();

	// �ر�socket
	bool doClose();
};

#endif