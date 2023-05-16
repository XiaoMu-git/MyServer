#include "Server.h"

// �޲ι���
Server::Server() {
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	_socket = INVALID_SOCKET;
	_timer.upData();
}

// ��������
Server::~Server() {
#ifdef _WIN32
	WSACleanup();
#endif
	doClose();
}

// ��ʼ��SOCKET
bool Server::doSocket() {
	if (isRun()) doClose();
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (isRun()) return true;
	else return false;
}

// �󶨶˿�
bool Server::doBind(const char* ip, unsigned short port) {
	if (!isRun()) doSocket();
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(port);
#ifdef _WIN32
	if (ip) _sin.sin_addr.S_un.S_addr = inet_addr(ip);
	else _sin.sin_addr.S_un.S_addr = INADDR_ANY;
#else
	if (ip) _sin.sin_addr.s_addr = inet_addr(ip);
	else _sin.sin_addr.s_addr = INADDR_ANY;
#endif
	int ret = bind(_socket, (sockaddr*)&_sin, sizeof(_sin));
	return ret != SOCKET_ERROR;
}

// �����˿�
bool Server::doListen(int n) {
	int ret = listen(_socket, n);
	return ret != SOCKET_ERROR;
}

// �ȴ��ͻ���
bool Server::doAccept() {
	sockaddr_in client_addr = {};
	int addr_len = sizeof(sockaddr_in);
	ClientInfo* client = new ClientInfo();
#ifdef _WIN32
	client->_socket = accept(_socket, (sockaddr*)&client_addr, &addr_len);
#else
	client->_socket = accept(_socket, (sockaddr*)&client_addr, (socklen_t*)&addr_len);
#endif
	if (client->_socket != INVALID_SOCKET) {
		assignedClient(client);
		return true;
	}
	else return false;
}

// ����������
bool Server::doRun() {
	timeval time_val = { 0, 10 };
	while (isRun()) {
		timeMsg();

		fd_set fd_reads;
		FD_ZERO(&fd_reads);
		FD_SET(_socket, &fd_reads);
		int ret = select(_socket + 1, &fd_reads, NULL, NULL, &time_val);
		if (ret < 0) return false;

		// �������������ô�����ȴ��ͻ���
		if (FD_ISSET(_socket, &fd_reads)) {
			FD_CLR(_socket, &fd_reads);
			doAccept();
		}
	}
	return false;
}

void Server::Start(int n) {
	for (int i = 0; i < n; i++) {
		CoreServer* server = new CoreServer(i, _socket);
		server->Start();
		_core_servers.push_back(server);
	}
	doRun();
}

// ����״̬
bool Server::isRun() {
	return _socket != INVALID_SOCKET;
}

// �رշ�����
bool Server::doClose() {
	if (isRun()) {
#ifdef _WIN32
		closesocket(_socket);
#else
		close(_socket);
#endif
		_socket = INVALID_SOCKET;
		return true;
	}
	return false;
}

// ����ͻ���
void Server::assignedClient(ClientInfo* client) {
	CoreServer* min_Server = NULL;
	for (auto server : _core_servers) {
		if (min_Server == NULL || min_Server->getClientNum() > server->getClientNum()) {
			min_Server = server;
		}
	}
	min_Server->addClient(client);
}

// �����������Ϣ
void Server::timeMsg() {
	float time_interval = _timer.getSec();
	if (time_interval >= 1.0f) {
		int client_all = 0;
		int recv_all = 0;
		int recv_pkg = 0;
		for (auto server : _core_servers) {
			client_all += server->getClientNum();
			recv_all += server->_recv_count;
			recv_pkg += server->_recv_package;
			server->_recv_count = 0;
			server->_recv_package = 0;
		}
		printf("����ˣ�thread<%d>,client<%d>,recv<%d>,package<%d>\n", _core_servers.size(), client_all, int(recv_all / time_interval), int(recv_pkg / time_interval));
		_timer.upData();
	}
}