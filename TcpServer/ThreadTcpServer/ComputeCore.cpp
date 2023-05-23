#include "ComputeCore.h"

// 有参构造
ComputeCore::ComputeCore(SOCKET socket) {
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	_thread = NULL;
	_socket = socket;
	_recv_num = _recv_pkg = 0;
}

// 析构函数
ComputeCore::~ComputeCore() {
#ifdef _WIN32
	WSACleanup();
#endif
	doClose();
}

// 接收消息
bool ComputeCore::doRecv(Client* client) {
	int recv_len = (int)recv(client->_socket, client->_recv_buff + client->_recv_pos, RECV_BUFF_SIZE - client->_recv_pos, 0);
	if (recv_len <= 0) return false;
	int left_pos = 0;
	client->_recv_pos += recv_len;
	_recv_num++;
	while (client->_recv_pos >= sizeof(Header)) {
		Header* header = (Header*)(client->_recv_buff + left_pos);
		if (client->_recv_pos >= header->_size) {
			doDispose(client, header);
			client->_recv_pos -= header->_size;
			left_pos += header->_size;
			_recv_pkg++;
		}
		else break;
	}
	memcpy(client->_recv_buff, client->_recv_buff + left_pos, client->_recv_pos);
	return true;
}

// 处理消息
void ComputeCore::doDispose(Client* client, Header* header) {
	short type = header->_type;
	if (type == CMD_MESSAGE) {
		_send_core.addTask(new SendTask(client, header));
	}
}

// 服务器运行
void ComputeCore::doRun() {
	_send_core.doThread();
	timeval time_val = { 0, 10 };
	bool is_change = false;
	while (isRun()) {
		if (_clients_buffer.size()) {
			std::lock_guard<std::mutex> lock(_mutex);
			for (auto client : _clients_buffer) _clients.push_back(client);
			_clients_buffer.clear();
			is_change = true;
		}

		fd_set fd_reads;
		FD_ZERO(&fd_reads);
		FD_SET(_socket, &fd_reads);
		SOCKET max_socket = _socket;
		for (auto client : _clients) {
			FD_SET(client->_socket, &fd_reads);
			if (max_socket < client->_socket) max_socket = client->_socket;
		}

		int ret = select(max_socket + 1, &fd_reads, NULL, NULL, &time_val);
		if (ret < 0) return;

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
}

// 开启服务器线程
void ComputeCore::doThread() {
	_thread = new std::thread(std::mem_fn(&ComputeCore::doRun), this);
	_thread->detach();
}

// 运行状态
bool ComputeCore::isRun() {
	return _socket != INVALID_SOCKET && _thread != NULL;
}

// 关闭服务器
void ComputeCore::doClose() {
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
		delete _thread;
		_clients.clear();
		_socket = INVALID_SOCKET;
	}
}

// 添加新的客户端
void ComputeCore::addClient(Client* client) {
	std::lock_guard<std::mutex> lock(_mutex);
	_clients_buffer.push_back(client);
}

// 获取客户端数量
int ComputeCore::getClientNum() {
	return _clients.size();
}

// 获取发送的次数
int ComputeCore::getSendNum() {
	int send_num = _send_core._send_num;
	_send_core._send_num = 0;
	return send_num;
}

// 获取发送包的数量
int ComputeCore::getSendPkg() {
	int send_pkg = _send_core._send_pkg;
	_send_core._send_pkg = 0;
	return send_pkg;
}
