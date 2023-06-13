#include "Server.h"

Server::Server(const char* ip, unsigned short port, int core_count) {
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	_ip = new char[strlen(ip)];
	strcpy(_ip, ip); 
	_port = port;
	_socket = INVALID_SOCKET;
	_core_count = core_count;
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
		shareClient(client);
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
		while (client->_recv_pos >= sizeof(Header)) {
			Header* header = (Header*)client->_recv_buff;
			if (client->_recv_pos >= header->_size) {
				client->_recv_pos -= header->_size;
				processMsg(client, header);
				memcpy(client->_recv_buff, client->_recv_buff + header->_size, client->_recv_pos);
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
	if (initSocket()) printf("socket ��ʼ���ɹ�...\n");
	else {
		printf("socket ��ʼ��ʧ��...\n");
		return;
	}
	if (doBind()) printf("�˿ڰ󶨳ɹ�...\n");
	else {
		printf("�˿ڰ�ʧ��...\n");
		return;
	}
	if (doListen()) printf("�˿ڼ����ɹ�...\n");
	else {
		printf("�˿ڼ���ʧ��...\n");
		return;
	}

	for (int i = 0; i < _core_count; i++) {
		ComputeCorePtr temp = std::make_shared<ComputeCore>(_socket);
		temp->doThread();
		_compute_cores.push_back(temp);
	}

	while (isRun()) {
		fd_set fd_reads;
		FD_ZERO(&fd_reads);
		FD_SET(_socket, &fd_reads);
		int ret = select(_socket + 1, &fd_reads, nullptr, nullptr, &time_val);
		if (ret < 0) return;

		if (FD_ISSET(_socket, &fd_reads)) {
			FD_CLR(_socket, &fd_reads);
			doAccept();
		}
	}
}

void Server::shareClient(ClientPtr& client) {
	ComputeCorePtr max_core = nullptr;
	for (auto core : _compute_cores) {
		if (max_core == nullptr || core->clientCount() < max_core->clientCount()) max_core = core;
	}
	if (max_core != nullptr) max_core->addClient(client);
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
		closesocket(_socket);
#else
		close(_socket);
#endif
		_socket = INVALID_SOCKET;
	}
}

size_t Server::clientCount() {
	size_t clientCount = 0;
	for (auto core : _compute_cores) clientCount += core->clientCount();
	return clientCount;
}

std::pair<int, int> Server::recvCount() {
	std::pair<int, int> res = { 0,0 };
	for (auto core : _compute_cores) {
		res.first += (int)core->_recv_count; core->_recv_count = 0;
		res.second += (int)core->_recv_pkg; core->_recv_pkg = 0;
	}
	return res;
}

std::pair<int, int> Server::sendCount() {
	std::pair<int, int> res = { 0,0 };
	for (auto core : _compute_cores) {
		res.first += (int)core->_send_count; core->_send_count = 0;
		res.second += (int)core->_send_pkg; core->_send_pkg = 0;
	}
	return res;
}
