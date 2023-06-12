#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "DataType.h"
#include "HighTimer.h"

class Client {
private:
	int _recv_pos, _send_pos;
	char* _recv_buff, * _send_buff;
	char _ip[10];
	unsigned short _port;
	SOCKET _socket;
	std::thread* _thread;

	// ��ʼ�� socket
	bool initSocket();

	// ���ӷ�����
	bool doConnect();

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

public:
	int _recv_count, _recv_pkg;
	int _send_count, _send_pkg;

	Client(const char ip[], unsigned short port);

	// �����߳�
	void doThread();

	~Client();
};

#endif // !_CLIENT_H_
