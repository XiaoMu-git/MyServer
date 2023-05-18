#ifndef Server
#define _Server_h_

#include "DataType.h"
#include "ComputeCore.h"
#include "HighTimer.h"

class Server {
private:
	SOCKET _socket;
	HighTimer _timer;
	std::vector<ComputeCore*> _core_servers;

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
	void assignedClient(Client* client);

	// �����������Ϣ
	void timeMsg();

};

#endif // !Server
