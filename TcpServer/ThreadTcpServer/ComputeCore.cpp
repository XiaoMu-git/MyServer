#include "ComputeCore.h"

// �вι���
ComputeCore::ComputeCore(int id, SOCKET socket) {
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	_id = id;
	_socket = socket;
	_recv_count = 0;
	_recv_package = 0;
}

// ��������
ComputeCore::~ComputeCore() {
#ifdef _WIN32
	WSACleanup();
#endif
	doClose();
}

// ������Ϣ
bool ComputeCore::doSend(SOCKET socket, Header* header) {
	if (isRun() && header) {
		int ret = send(socket, (const char*)header, header->_length, 0);
		return ret > 0;
	}
	return false;
}

// ������Ϣ
bool ComputeCore::doRecv(Client* client) {
	int recv_len = (int)recv(client->_socket, client->_recv_buff + client->_recv_data_len, RECV_BUFF_SIZE - client->_recv_data_len, 0);
	if (recv_len <= 0) return false;
	int left_pos = 0;
	client->_recv_data_len += recv_len;
	_recv_count++;
	while (client->_recv_data_len >= sizeof(Header)) {
		Header* header = (Header*)(client->_recv_buff + left_pos);
		if (client->_recv_data_len >= header->_length) {
			doDispose(client->_socket, header);
			client->_recv_data_len -= header->_length;
			left_pos += header->_length;
			_recv_package++;
		}
		else break;
	}
	memcpy(client->_recv_buff, client->_recv_buff + left_pos, client->_recv_data_len);
	return true;
}

// ������Ϣ
bool ComputeCore::doDispose(SOCKET client_sock, Header* header) {
	short type = header->_type;
	return true;
}

// ����������
bool ComputeCore::doRun() {
	timeval time_val = { 0, 10 };
	bool is_change = false;
	while (isRun()) {
		// ȡ��������еĿͻ���
		if (_clients_buffer.size()) {
			std::lock_guard<std::mutex> lock(_mutex);
			for (auto client : _clients_buffer) _clients.push_back(client);
			_clients_buffer.clear();
			is_change = true;
		}

		// ���� fd_set
		fd_set fd_reads;
		FD_ZERO(&fd_reads);
		FD_SET(_socket, &fd_reads);
		SOCKET max_socket = _socket;
		for (auto client : _clients) {
			FD_SET(client->_socket, &fd_reads);
			if (max_socket < client->_socket) max_socket = client->_socket;
		}

		// ���� select ģ��
		int ret = select(max_socket + 1, &fd_reads, NULL, NULL, &time_val);
		if (ret < 0) return false;

		// ɾ���Ͽ����ӵĿͻ���
		for (auto client : _clients) {
			if (FD_ISSET(client->_socket, &fd_reads) && !doRecv(client)) {
				for (auto iter = _clients.begin(); iter != _clients.end(); iter++) {
					if ((*iter)->_socket == client->_socket) {
						delete client;
						_clients.erase(iter);
						break;
					}
				}
				is_change = true;
			}
		}
	}
	return false;
}

// �����������߳�
void ComputeCore::Start() {
	_thread = std::thread(std::mem_fn(&ComputeCore::doRun), this);
	_thread.detach();
}

// ����״̬
bool ComputeCore::isRun() {
	return _socket != INVALID_SOCKET;
}

// �رշ�����
bool ComputeCore::doClose() {
	if (isRun()) {
#ifdef _WIN32
		for (auto client : _clients) {
			closesocket(client->_socket);
			delete client;
		}
		closesocket(_socket);
#else
		for (auto client : _clients) {
			close(client->_socket);
			delete client;
		}
		close(_socket);
#endif
		_clients.clear();
		_socket = INVALID_SOCKET;
		return true;
	}
	return false;
}

// ����µĿͻ���
void ComputeCore::addClient(Client* client) {
	std::lock_guard<std::mutex> lock(_mutex);
	_clients_buffer.push_back(client);
}

// ��ȡ�ͻ�������
int ComputeCore::getClientNum() {
	return _clients.size();
}

// ��ȡ�߳� id
int ComputeCore::getId() {
	return _id;
}
