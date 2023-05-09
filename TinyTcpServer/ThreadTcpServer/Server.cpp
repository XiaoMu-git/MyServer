#include "Server.h"

Server::Server() {
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	_socket;
	_clients;
	_buffer;
	_buffer_len;
}

Server::~Server() {

}

bool Server::doInit() {

}

bool Server::doBind(const char* ip, unsigned short port) {

}

bool Server::doListen(int num) {

}

bool Server::doAccept() {

}

bool Server::doSend(SOCKET socket, Header* header) {

}

bool Server::doRecv(ClientInfo* client) {

}

bool Server::doDispose(SOCKET client_sock, Header* header) {

}

bool Server::doRun(timeval time_val) {

}

bool Server::isRun() {

}

bool Server::doClose() {

}