#include "CoreServer.h"

// �вι���
CoreServer::CoreServer(int id, SOCKET socket) {
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	_id = id;
	_socket = socket;
}

// ��������
CoreServer::~CoreServer() {
#ifdef _WIN32
	WSACleanup();
#endif
	doClose();
}

// ������Ϣ
bool CoreServer::doSend(SOCKET socket, Header* header) {
	if (isRun() && header) {
		int ret = send(socket, (const char*)header, header->_length, 0);
		return ret > 0;
	}
	return false;
}

// ������Ϣ
bool CoreServer::doRecv(ClientInfo* client) {
	int len = (int)recv(client->_socket, client->_buffer + client->_data_len, BUFF_SIZE - client->_data_len, 0);
	if (len <= 0) return false;
	client->_data_len += len;
	while (client->_data_len >= sizeof(Header)) {
		Header* header = (Header*)client->_buffer;
		if (client->_data_len >= header->_length) {
			client->_data_len -= header->_length;
			doDispose(client->_socket, header);
			memcpy(client->_buffer, client->_buffer + header->_length, client->_data_len);
		}
		else break;
	}
	return true;
}

// ������Ϣ
bool CoreServer::doDispose(SOCKET client_sock, Header* header) {
	short type = header->_type;
	return true;
}

// ����������
bool CoreServer::doRun() {
	timeval time_val = { 0, 10 };
	while (isRun()) {
		// ȡ��������еĿͻ���
		if (_clients_buffer.size()) {
			std::lock_guard<std::mutex> lock(_mutex);
			for (auto client : _clients_buffer) {
				_clients.push_back(client);
				printf("thread<%d>, socket<%d>, Establish a connection\n", _id, client->_socket);
			}
			_clients_buffer.clear();
		}

		// ���û����Ҫ����Ŀͻ��ˣ���������
		if (_clients.empty()) {
			std::chrono::milliseconds timer(1);
			std::this_thread::sleep_for(timer);
			continue;
		}

		fd_set fd_reads;
		FD_ZERO(&fd_reads);
		FD_SET(_socket, &fd_reads);

		// ���������ӵĿͻ���
		SOCKET max_socket = _socket;
		for (auto client : _clients) {
			FD_SET(client->_socket, &fd_reads);
			max_socket = max(max_socket, client->_socket);
		}
		int ret = select(max_socket + 1, &fd_reads, NULL, NULL, &time_val);
		if (ret < 0) return false;

		// ɾ���Ͽ����ӵĿͻ���
		for (auto client : _clients) {
			if (FD_ISSET(client->_socket, &fd_reads) && !doRecv(client)) {
				auto iter = find(_clients.begin(), _clients.end(), client);
				if (iter != _clients.end()) {
					printf("thread<%d>, socket<%d>, Disconnect��\n", _id, client->_socket);
					delete client;
					_clients.erase(iter);
				}
			}
		}
	}
	return false;
}

void CoreServer::Start() {
	_thread = std::thread(std::mem_fn(&CoreServer::doRun), this);
	_thread.detach();
}

// ����״̬
bool CoreServer::isRun() {
	return _socket != INVALID_SOCKET;
}

// �رշ�����
bool CoreServer::doClose() {
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
void CoreServer::addClient(ClientInfo* client) {
	std::lock_guard<std::mutex> lock(_mutex);
	_clients_buffer.push_back(client);
}

// ��ȡ�ͻ�������
int CoreServer::getClientNum() {
	return _clients_buffer.size();
}

// ��ȡ�߳� id
int CoreServer::getId() {
	return _id;
}
