#ifndef _Client_h_
#define _Client_h_

#include "DataPackage.h"

class Client {
private:
	SOCKET _socket;
	sockaddr_in _sockaddr;

public:
	Client();

	virtual ~Client();

	// ��ʼ��socket
	bool initSocket();

	// ���ӷ�����
	bool doConnect(const char* ip, int post);

	// �ر�socket
	bool doClose();

	// 
};

#endif