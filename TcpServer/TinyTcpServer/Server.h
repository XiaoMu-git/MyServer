#ifndef _SERVER_H_
#define _SERVER_H_

#include "DataType.h"
#include "HighTimer.h"

class Server {
private:
	const char* _ip;
	unsigned short _port;
	SOCKET _socket;
	HighTimer _timer;
	std::mutex _mutex;
	std::map<SOCKET, Client*> _clients;

protected:
	// ��ʼ�� socket
	bool initSocket();

	// �󶨶˿�
	bool doBind();

	// �����˿�
	bool doListen(int num);

	// �ȴ��ͻ�������
	void waitAccept();

	// ������Ϣ
	int doRecv(Client* client);

	// ������Ϣ
	void doSend(Client* client, Header* header);

	// ������Ϣ
	void doDispose(Client* client, Header* header);

	// ���к���
	void doRun();

	// �رշ�����
	void doClose();

	// ����״̬
	bool isRun();

	// ��������Ϣ
	void printInfo();

public:
	std::atomic_int _recv_num, _recv_pkg;
	std::atomic_int _send_num, _send_pkg;

	Server(const char* ip, unsigned short port);

	// ����������
	void Start();

	virtual ~Server();
};

#endif // !_SERVER_H_
