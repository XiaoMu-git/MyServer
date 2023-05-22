#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "DataType.h"

class Client {
private:
	const char* _ip;
	unsigned short _port;
	char* _recv_buff;
	char* _send_buff;
	int _recv_pos, _send_pos;

protected:
	// ��ʼ��socket
	bool initSocket();

	// ���ӷ�����
	bool doConnect();

	// ������Ϣ
	int doRecv();

	// ������Ϣ
	void doSend(Header* header);

	// ������Ϣ
	void doDispose(Header* header);

	// ����״̬
	bool isRun();

	// �رտͻ���
	void doClose();

public:
	SOCKET _socket;
	std::atomic_int _recv_num, _recv_pkg;
	std::atomic_int _send_num, _send_pkg;

	Client(const char* ip, unsigned short port);

	// ���к���
	bool doRun(Header* header);

	// �����ͻ���
	void Start();

	virtual ~Client();
};

#endif // !_CLIENT_H_
