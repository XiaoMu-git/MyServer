#include "ComputeCore.h"

ComputeCore::ComputeCore(SOCKET socket) {
	_socket = socket;
	_is_change = true;
}

ComputeCore::~ComputeCore() {
	doClose();
}

void ComputeCore::doSend(ClientPtr& client, HeaderPtr& header) {
	TaskPtr task = std::make_shared<SendTask>(client, header);
	_task_core.addTask(task);
}

int ComputeCore::doRecv(ClientPtr& client) {
	int len = (int)recv(client->_socket, client->_recv_buff + client->_recv_pos, RECV_BUFF_SIZE - client->_recv_pos, 0);
	if (len > 0) {
		client->_recv_pos += len;
		_recv_count++;
		while (client->_recv_pos >= sizeof(Header)) {
			Header* header = (Header*)client->_recv_buff;
			if (client->_recv_pos >= header->_size) {
				client->_recv_pos -= header->_size;
				processMsg(client, header);
				memcpy(client->_recv_buff, client->_recv_buff + header->_size, client->_recv_pos);
				_recv_pkg++;
			}
			else break;
		}
	}
	return len;
}

void ComputeCore::processMsg(ClientPtr& client, Header* header) {
	short type = header->_type;
}

void ComputeCore::doWork() {
	timeval time_val = { 0, 10 };
	SOCKET max_socket = _socket;
	fd_set fd_back, fd_reads;
	_task_core.Thread();
	while (isRun()) {
		if (!_clients_buff.empty()) {
			std::lock_guard<std::mutex> lock(_mutex);
			for (auto client : _clients_buff) _clients.push_back(client);
			_clients_buff.clear();
			_is_change = true;
		}

		FD_ZERO(&fd_reads);
		FD_SET(_socket, &fd_reads);
		if (_is_change) {
			_is_change = false;
			for (auto client : _clients) {
				FD_SET(client->_socket, &fd_reads);
				max_socket = max(max_socket, client->_socket);
			}
			memcpy(&fd_back, &fd_reads, sizeof(fd_set));
		}
		else memcpy(&fd_reads, &fd_back, sizeof(fd_set));

		
		int ret = select(max_socket + 1, &fd_reads, nullptr, nullptr, &time_val);
		if (ret < 0) return;
		else if (ret == 0) continue;

		for (auto client : _clients) {
			if (client == nullptr) continue;
			if (FD_ISSET(client->_socket, &fd_reads) && doRecv(client) < 0) {
				for (auto it = _clients.begin(); it != _clients.end(); it++) {
					if ((*it)->_socket == client->_socket) {
						_clients.erase(it);
						break;
					}
				}
				_is_change = true;
			}
		}
	}
}

void ComputeCore::doThread() {
	_thread = std::thread(std::mem_fn(&ComputeCore::doWork), this);
	_thread.detach();
}

bool ComputeCore::isRun() {
	return _socket != INVALID_SOCKET;
}

void ComputeCore::addClient(ClientPtr& client) {
	std::lock_guard<std::mutex> lock(_mutex);
	_clients_buff.push_back(client);
}

void ComputeCore::doClose() {
	if (isRun()) {
#ifdef _WIN32
		for (auto client : _clients_buff) closesocket(client->_socket);
		for (auto client : _clients) closesocket(client->_socket);
		closesocket(_socket);
#else
		for (auto client : _clients_buff) close(client->_socket);
		for (auto client : _clients) close(client->_socket);
		close(_socket);
#endif
		_clients.clear();
		_clients_buff.clear();
		_socket = INVALID_SOCKET;
	}
}

size_t ComputeCore::clientCount() {
	return _clients.size();
}
