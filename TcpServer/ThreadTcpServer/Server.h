#ifndef Server
#define _Server_h_

#include "DataType.h"
#include "CoreServer.h"

class Server {
private:
	HighTimer _timer;
	SOCKET _socket;
	std::vector<CoreServer*> core_servers;

public:
	// �޲ι���
	Server();

	// ��������
	~Server();

	// ��ʼ��SOCKET
	bool doSocket();

	// �󶨶˿�
	bool doBind(const char* ip, unsigned short port);

	// �����˿�
	bool doListen(int n = 5);

	// �ȴ��ͻ���
	bool doAccept();

	// ����������
	bool doRun();

	// ����������
	void Start(int n = 1);

	// ����״̬
	bool isRun();

	// �رշ�����
	bool doClose();

	// ����ͻ���
	void assignedClient(ClientInfo* client);

	// ��������Ϣ
	void timerMsg();
};

#endif // !Server
