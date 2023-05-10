#include "DisposeServer.h"

DisposeServer::DisposeServer() {
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	_socket = INVALID_SOCKET;
	_buffer = new char[BUFF_SIZE];
	_buffer_len = 0;
}

DisposeServer::~DisposeServer() {
#ifdef _WIN32
	WSACleanup();
#endif
	doClose();
}

bool DisposeServer::doInit() {
	if (isRun()) doClose();
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (isRun()) return true;
	else return false;
}

bool DisposeServer::doRun(timeval time_val) {
	if (!isRun() && !doInit()) return false;
	while (isRun()) {
		// 从缓冲队列里取出客户数据
		if (_clients_buffer.size() > 0) {
			lock_guard<mutex> lock(_mutex);
			for (auto client : _clients_buffer) {
				_clients.push_back(client);
			}
			_clients_buffer.clear();
		}

		// 没有客户端则跳过
		if (_clients.empty()) {
			chrono::milliseconds sleep(1);
			this_thread::sleep_for(sleep);
			continue;
		}

		fd_set fd_reads;
		FD_ZERO(&fd_reads);
		SOCKET maxSock = INVALID_SOCKET;
		for (auto client : _clients) {
			FD_SET(client->_socket, &fd_reads);
			if (maxSock < client->_socket) {
				maxSock = client->_socket;
			}
		}

		int ret = select(maxSock + 1, &fd_reads, NULL, NULL, NULL);
		if (ret < 0) {
			printf("select任务结束。\n");
			doClose();
			return false;
		}

		for (auto client : _clients) {
			if (FD_ISSET(client->_socket, &fd_reads) && !doRecv(client)) {				
				auto iter = find(_clients.begin(), _clients.end(), client);
				if (iter != _clients.end()) {
					printf("客户端socket<%d>与服务器断开连接\n", client->_socket);
					delete client;
					_clients.erase(iter);
					break;
				}
			}
		}
	}
}

bool DisposeServer::isRun() {
	return _socket != INVALID_SOCKET;
}

bool DisposeServer::doRecv(ClientInfo* client) {
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

bool DisposeServer::doSend(SOCKET socket, Header* header) {
	if (isRun() && header) {
		int ret = send(socket, (const char*)header, header->_length, 0);
		return ret > 0;
	}
	return false;
}

bool DisposeServer::doDispose(SOCKET client_sock, Header* header) {
	short type = header->_type;
	return true;
}

bool DisposeServer::doClose() {
	if (_socket != INVALID_SOCKET)
	{
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
		close(_sock);
#endif
		_clients.clear();
		_socket == INVALID_SOCKET;
		return true;
	}
	return false;
}

void DisposeServer::doStart() {
	_thread = new thread(mem_fn(&DisposeServer::doRun), this);
}

int DisposeServer::getClientsBufferLen() {
	return _clients_buffer.size();
}

void DisposeServer::addClientBuffer(ClientInfo* client) {
	_clients_buffer.push_back(client);
}