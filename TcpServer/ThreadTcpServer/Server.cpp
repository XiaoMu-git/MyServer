#include "Server.h"

Server::Server(const char* ip, unsigned short port) {
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	_ip = new char[strlen(ip)];
	strcpy(_ip, ip); 
	_port = port;
	_socket = INVALID_SOCKET;
	_recv_count = _recv_pkg = 0;
	_send_count = _send_pkg = 0;
}

Server::~Server() {
#ifdef _WIN32
	WSACleanup();
#endif
	doClose();
}

bool Server::initSocket() {
	if (isRun()) doClose();
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (isRun()) return true;
	else return false;
}

bool Server::doBind() {
	if (!isRun()) initSocket();
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(_port);
#ifdef _WIN32
	if (_ip) _sin.sin_addr.S_un.S_addr = inet_addr((const char*)_ip);
	else _sin.sin_addr.S_un.S_addr = INADDR_ANY;
#else
	if (_ip) _sin.sin_addr.s_addr = inet_addr((const char*)_ip);
	else _sin.sin_addr.s_addr = INADDR_ANY;
#endif
	int ret = bind(_socket, (sockaddr*)&_sin, sizeof(_sin));
	return ret != SOCKET_ERROR;
}

bool Server::doListen() {
	int ret = listen(_socket, 5);
	return ret != SOCKET_ERROR;
}

void Server::doAccept() {
	sockaddr_in client_addr = {};
	int addr_len = sizeof(sockaddr_in);
	SOCKET temp = INVALID_SOCKET;
#ifdef _WIN32
	temp = accept(_socket, (sockaddr*)&client_addr, &addr_len);
#else
	temp = accept(_socket, (sockaddr*)&client_addr, (socklen_t*)&addr_len);
#endif
	if (temp != INVALID_SOCKET) {
		ClientPtr client = std::make_shared<Client>(temp);
		_clients.push_back(client);
	}
}

void Server::doSend(ClientPtr& client, HeaderPtr& header) {
	if (isRun() && header.get()) {
		int ret = send(client->_socket, (const char*)header.get(), header->_size, 0);
	}
}

int Server::doRecv(ClientPtr& client) {
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

void Server::processMsg(ClientPtr& client, Header* header) {
	short type = header->_type;
}

void Server::doWork() {
	timeval time_val = { 0,10 };
	if (initSocket()) printf("socket 初始化成功...\n");
	else {
		printf("socket 初始化失败...\n");
		return;
	}
	if (doBind()) printf("端口绑定成功...\n");
	else {
		printf("端口绑定失败...\n");
		return;
	}
	if (doListen()) printf("端口监听成功...\n");
	else {
		printf("端口监听失败...\n");
		return;
	}

	while (isRun()) {
		fd_set fd_reads;
		FD_ZERO(&fd_reads);
		FD_SET(_socket, &fd_reads);

		SOCKET max_socket = _socket;
		for (auto client : _clients) {
			FD_SET(client->_socket, &fd_reads);
			max_socket = max(max_socket, client->_socket);
		}
		int ret = select(max_socket + 1, &fd_reads, NULL, NULL, &time_val);
		if (ret < 0) return;

		if (FD_ISSET(_socket, &fd_reads)) {
			FD_CLR(_socket, &fd_reads);
			doAccept();
			continue;
		}

		for (auto client : _clients) {
			if (client == nullptr) continue;
			if (FD_ISSET(client->_socket, &fd_reads) && doRecv(client) < 0) {
				for (auto it = _clients.begin(); it != _clients.end(); it++) {
					if ((*it)->_socket == client->_socket) {
						_clients.erase(it);
						break;
					}
				}
			}
		}
	}
}

void Server::doThread() {
	_thread = std::thread(std::mem_fn(&Server::doWork), this);
	_thread.detach();
}

bool Server::isRun() {
	return _socket != INVALID_SOCKET;
}

void Server::doClose() {
	if (isRun()) {
#ifdef _WIN32
		for (auto client : _clients) {
			closesocket(client->_socket);
		}
		closesocket(_socket);
#else
		for (auto client : _clients) {
			close(client->_socket);
		}
		close(_socket);
#endif
		_clients.clear();
		_socket = INVALID_SOCKET;
	}
}

size_t Server::clientCount() {
	return _clients.size();
}
