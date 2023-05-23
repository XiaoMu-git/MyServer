#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "DataType.h"
#include "HighTimer.h"

class Client {
private:
	SOCKET _socket;
	char* _recv_buff;
	char* _send_buff;
	int _recv_pos, _send_pos;
	bool _connect;

public:
	Client();

	// ��ʼ�� socket
	bool initSocket();

	// ���ӷ�����
	bool doConnect(const char* ip, unsigned short port);

	// ������Ϣ
	void doSend(Header* header);

	// ������Ϣ
	int doRecv();
	
	// ������Ϣ
	void doDispose(Header* header);

	// ���к���
	void doRun();

	// ����״̬
	bool isRun();

	// �رտͻ���
	void doClose();

	// _socket �ӿ�
	SOCKET Socket(const SOCKET socket = -1);

	// _connect �ӿ�
	bool Connect(int connect = -1);

	~Client();
};

#endif // !_CLIENT_H_
