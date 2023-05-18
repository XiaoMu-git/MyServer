#ifndef _CORESERVER_H_
#define _CORESERVER_H_

#include "DataType.h"
#include "Client.h"
#include <functional>
#include <mutex>
#include <vector>
#include <atomic>

class ComputeCore {
private:
	int _id;
	fd_set fd_reads_back;
	SOCKET _socket;
	std::mutex _mutex;
	std::thread _thread;
	std::vector<Client*> _clients;
	std::vector<Client*> _clients_buffer;

public:
	std::atomic_int _recv_package;
	std::atomic_int _recv_count;

	// �вι���
	ComputeCore(int id, SOCKET socket = INVALID_SOCKET);

	// ��������
	~ComputeCore();

	// ������Ϣ
	bool doSend(SOCKET socket, Header* header);

	// ������Ϣ
	bool doRecv(Client* client);

	// ������Ϣ
	bool doDispose(SOCKET client_sock, Header* header);

	// ����������
	bool doRun();

	// �����������߳�
	void Start();

	// ����״̬
	bool isRun();

	// �رշ�����
	bool doClose();

	// ����µĿͻ���
	void addClient(Client* client);

	// ��ȡ�ͻ�������
	int getClientNum();

	// ��ȡ�߳� id
	int getId();

};

#endif // !_CORESERVER_H_
