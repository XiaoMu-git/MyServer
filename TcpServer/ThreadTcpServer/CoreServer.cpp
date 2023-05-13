#include "CoreServer.h"

// 有参构造
CoreServer::CoreServer(int id, SOCKET socket) {
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	_id = id;
	_socket = socket;
	_recv_count = 0;
}

// 析构函数
CoreServer::~CoreServer() {
#ifdef _WIN32
	WSACleanup();
#endif
	doClose();
}

// 发送消息
bool CoreServer::doSend(SOCKET socket, Header* header) {
	if (isRun() && header) {
		int ret = send(socket, (const char*)header, header->_length, 0);
		return ret > 0;
	}
	return false;
}

// 接收消息
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
			_recv_count++;
		}
		else break;
	}
	return true;
}

// 处理消息
bool CoreServer::doDispose(SOCKET client_sock, Header* header) {
	short type = header->_type;
	return true;
}

// 服务器运行
bool CoreServer::doRun() {
	fd_set fd_reads_back;
	bool fd_reads_change = true;
	while (isRun()) {
		// 取出缓存队列的客户端
		if (_clients_buffer.size()) {
			std::lock_guard<std::mutex> lock(_mutex);
			for (auto client : _clients_buffer) {
				_clients.insert({ client->_socket,client });
				if (_max_socket < client->_socket) _max_socket = client->_socket;
			}
			_clients_buffer.clear();
			fd_reads_change = true;
		}

		// 加入新连接的客户端
		fd_set fd_reads;
		FD_ZERO(&fd_reads);
		if (fd_reads_change) {
			FD_SET(_socket, &fd_reads);
			for (auto client : _clients) FD_SET(client.first, &fd_reads);
			memcpy(&fd_reads_back, &fd_reads, sizeof(fd_set));
			fd_reads_change = false;
		}
		else {
			memcpy(&fd_reads, &fd_reads_back, sizeof(fd_set));
		}

		// select 调用模型
		int ret = select(_max_socket + 1, &fd_reads, NULL, NULL, NULL);
		if (ret < 0) return false;

		// 删除断开连接的客户端
#ifdef _WIN32
		for (int i = 0; i < fd_reads.fd_count; i++) {
			auto iter = _clients.find(fd_reads.fd_array[i]);
			if (iter != _clients.end()) {
				if (!doRecv(iter->second)) {
					delete iter->second;
					_clients.erase(iter);
					fd_reads_change = true;
				}
			}
		}
#else
		std::vector<SOCKET> temp;
		for (auto client : _clients) {
			if (FD_ISSET(client.first, &fd_reads)) {
				if (!doRecv(client.second)) {
					temp.push_back(client.first);
					delete client.second;
					fd_reads_change = true;
				}
			}
		}
		for (auto sock : temp) _clients.erase(sock);
#endif
	}
	return false;
}

void CoreServer::Start() {
	_thread = std::thread(std::mem_fn(&CoreServer::doRun), this);
	_thread.detach();
}

// 运行状态
bool CoreServer::isRun() {
	return _socket != INVALID_SOCKET;
}

// 关闭服务器
bool CoreServer::doClose() {
	if (isRun()) {
#ifdef _WIN32
		for (auto client : _clients) {
			closesocket(client.first);
			delete client.second;
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

// 添加新的客户端进入缓存
void CoreServer::addClient(ClientInfo* client) {
	std::lock_guard<std::mutex> lock(_mutex);
	_clients_buffer.push_back(client);
}

// 获取客户端数量
int CoreServer::getClientNum() {
	return _clients.size();
}

// 获取线程 id
int CoreServer::getId() {
	return _id;
}
