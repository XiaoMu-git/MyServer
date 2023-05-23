#include "Server.h"

// 无参构造
Server::Server(const char* ip, unsigned short port) {
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	_ip = ip;
	_port = port;
	_socket = INVALID_SOCKET;
	_timer.upData();
}

// 析构函数
Server::~Server() {
#ifdef _WIN32
	WSACleanup();
#endif
	doClose();
}

// 初始化SOCKET
bool Server::initSocket() {
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	return _socket != INVALID_SOCKET;
}

// 绑定端口
bool Server::doBind() {
	sockaddr_in addr_in = {};
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(_port);
#ifdef _WIN32
	if (_ip) addr_in.sin_addr.S_un.S_addr = inet_addr(_ip);
	else addr_in.sin_addr.S_un.S_addr = INADDR_ANY;
#else
	if (_ip) addr_in.sin_addr.s_addr = inet_addr(_ip);
	else addr_in.sin_addr.s_addr = INADDR_ANY;
#endif
	int ret = bind(_socket, (sockaddr*)&addr_in, sizeof(addr_in));
	return ret != SOCKET_ERROR;
}

// 监听端口
bool Server::doListen(int num) {
	int ret = listen(_socket, num);
	return ret != SOCKET_ERROR;
}

// 等待客户端
void Server::waitAccept() {
	sockaddr_in client_addr = {};
	int addr_len = sizeof(sockaddr_in);
	SOCKET client_socket = INVALID_SOCKET;
#ifdef _WIN32
	client_socket = accept(_socket, (sockaddr*)&client_addr, &addr_len);
#else
	client_socket = accept(_socket, (sockaddr*)&client_addr, (socklen_t*)&addr_len);
#endif
	if (client_socket != INVALID_SOCKET) assignedClient(new Client(client_socket));
}

// 服务器运行
void Server::doRun() {
	timeval time_val = { 0, 10 };
	while (isRun()) {
		timeMsg();

		fd_set fd_reads;
		FD_ZERO(&fd_reads);
		FD_SET(_socket, &fd_reads);
		int ret = select(_socket + 1, &fd_reads, NULL, NULL, &time_val);
		if (ret < 0) return;

		if (FD_ISSET(_socket, &fd_reads)) {
			FD_CLR(_socket, &fd_reads);
			waitAccept();
		}
	}
}

// 开启服务器
void Server::Start(int n) {
	if (initSocket()) printf("socket初始化成功...\n");
	else {
		printf("socket初始化失败..\n");
		return;
	}

	if (doBind()) printf("绑定端口成功...\n");
	else {
		printf("绑定端口失败..\n");
		return;
	}

	if (doListen(5)) printf("监听端口成功...\n");
	else {
		printf("监听端口失败..\n");
		return;
	}

	while (n--) {
		ComputeCore* core = new ComputeCore(_socket);
		core->doThread();
		_compute_core.push_back(core);
	}
	doRun();
}

// 运行状态
bool Server::isRun() {
	return _socket != INVALID_SOCKET;
}

// 关闭服务器
bool Server::doClose() {
	if (isRun()) {
#ifdef _WIN32
		closesocket(_socket);
#else
		close(_socket);
#endif
		for (auto core : _compute_core) delete core;
		_socket = INVALID_SOCKET;
		return true;
	}
	return false;
}

// 分配客户端
void Server::assignedClient(Client* client) {
	ComputeCore* min_core = NULL;
	for (auto core : _compute_core) {
		if (!min_core || min_core->getClientNum() > core->getClientNum()) {
			min_core = core;
		}
	}
	min_core->addClient(client);
}

// 处理服务器消息
void Server::timeMsg() {
	float time_interval = _timer.getSec();
	if (time_interval >= 1.0f) {
		int client_num = 0;
		int recv_num = 0, recv_pkg = 0;
		int send_num = 0, send_pkg = 0;
		for (auto core : _compute_core) {
			client_num += core->getClientNum();
			recv_num += core->_recv_num; core->_recv_num = 0;
			recv_pkg += core->_recv_pkg; core->_recv_pkg = 0;
			send_num += core->getSendNum();
			send_pkg += core->getSendPkg();
		}
		printf("服务端：thread<%d>,client<%d>,recv_num<%d>,recv_pkg<%d>,send_num<%d>,send_pkg<%d>\n", _compute_core.size(), client_num, int(recv_num / time_interval), int(recv_pkg / time_interval), int(send_num / time_interval), int(send_pkg / time_interval));
		_timer.upData();
	}
}
