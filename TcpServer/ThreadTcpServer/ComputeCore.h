#ifndef _CORESERVER_H_
#define _CORESERVER_H_

#include "SendCore.h"

class ComputeCore {
private:
	SOCKET _socket;
	fd_set fd_reads_back;
	SendCore _send_core;
	std::mutex _mutex;
	std::thread* _thread;
	std::vector<Client*> _clients;
	std::vector<Client*> _clients_buffer;

protected:
	// ������Ϣ
	bool doRecv(Client* client);

	// ������Ϣ
	void doDispose(Client* client, Header* header);

	// ����������
	void doRun();

	// ����״̬
	bool isRun();

	// �رշ�����
	void doClose();

public:
	std::atomic_int _recv_num, _recv_pkg;
	
	ComputeCore(SOCKET socket);

	// ���������߳�
	void doThread();

	// ����µĿͻ���
	void addClient(Client* client);

	// ��ȡ�ͻ�������
	int getClientNum();

	// ��ȡ���͵Ĵ���
	int getSendNum();

	// ��ȡ���Ͱ�������
	int getSendPkg();

	~ComputeCore();
};

#endif // !_CORESERVER_H_
