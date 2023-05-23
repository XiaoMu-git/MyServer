#ifndef Server
#define _Server_h_

#include "DataType.h"
#include "ComputeCore.h"
#include "HighTimer.h"

class Server {
private:
	const char* _ip;
	unsigned short _port;
	SOCKET _socket;
	HighTimer _timer;
	std::vector<ComputeCore*> _compute_core;

protected:
	// ��ʼ�� socket
	bool initSocket();

	// �󶨶˿�
	bool doBind();

	// �����˿�
	bool doListen(int num);

	// �ȴ��ͻ���
	void waitAccept();

	// ����������
	void doRun();

	// ����״̬
	bool isRun();

	// �رշ�����
	bool doClose();

	// ����ͻ���
	void assignedClient(Client* client);

	// �����������Ϣ
	void timeMsg();

public:
	Server(const char* ip, unsigned short port);

	// ����������
	void Start(int n = 1);

	~Server();
};

#endif // !Server
