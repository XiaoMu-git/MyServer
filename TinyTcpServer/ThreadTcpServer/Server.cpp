#include "Server.h"

// 无参构造
Server::Server() {
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	_socket = INVALID_SOCKET;
}

// 析构函数
Server::~Server() {
#ifdef _WIN32
	WSACleanup();
#endif
	doClose();
}

// 初始化 socket
bool Server::doInit() {
	if (isRun()) doClose();
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (isRun()) return true;
	else return false;
}

// 运行状态
bool Server::isRun() {
	return _socket != INVALID_SOCKET;
}

// 关闭服务器
bool Server::doClose() {
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