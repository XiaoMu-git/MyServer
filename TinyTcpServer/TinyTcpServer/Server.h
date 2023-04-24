#ifndef _Server_h_
#define _Server_h_

#include "DataPackage.h"

class Server {
private:
	SOCKET _socket;
	sockaddr_in _addr;
	vector<UserInfo> _clients_info;

public:
	Server();	// ���캯��

	bool InitSocket();	// ��ʼ��socket

	bool toBind();	// �󶨶˿�

	bool toListen();	// �����˿�

	bool closeSocket();	// �ر�socket

	bool disposeReq();	// ��������

	~Server();	// ��������
};

#endif