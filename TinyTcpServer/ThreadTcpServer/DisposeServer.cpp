#include "DisposeServer.h"

DisposeServer::DisposeServer() {
	_buffer = new char[BUFF_SIZE];
	_buffer_len = 0;
}

DisposeServer::~DisposeServer() {

}

bool DisposeServer::doSend(SOCKET socket, Header* header) {

}

bool DisposeServer::doRun(timeval time_val) {

}

bool DisposeServer::doRecv(ClientInfo* client) {

}

bool DisposeServer::doDispose(SOCKET client_sock, Header* header) {

}