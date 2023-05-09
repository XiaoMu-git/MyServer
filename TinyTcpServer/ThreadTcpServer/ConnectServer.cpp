#include "ConnectServer.h"

ConnectServer::ConnectServer() {
	for (int i = 0; i < DISPOSESERVER_NUM; i++) _postServer[i] = new DisposeServer();
}

ConnectServer::~ConnectServer() {
	delete[] _postServer;
}

bool ConnectServer::Accept() {
	sockaddr_in client_addr = {};
	int addr_len = sizeof(sockaddr_in);
	ClientInfo* client = new ClientInfo();
#ifdef _WIN32
	client->_socket = accept(_socket, (sockaddr*)&client_addr, &addr_len);
#else
	client->_socket = accept(_socket, (sockaddr*)&client_addr, (socklen_t*)&addr_len);
#endif
	if (client->_socket != INVALID_SOCKET) {
		_clients.push_back(client);
		return true;
	}
	else return false;
}

bool ConnectServer::doRun(timeval time_val) {
	if (isRun()) {
		fd_set fd_reads;
		FD_ZERO(&fd_reads);
		FD_SET(_socket, &fd_reads);

		// ���������ӵĿͻ���
		SOCKET max_socket = _socket;
		for (auto client : _clients) {
			FD_SET(client->_socket, &fd_reads);
			max_socket = max(max_socket, client->_socket);
		}
		int ret = select(max_socket + 1, &fd_reads, NULL, NULL, &time_val);
		if (ret < 0) return false;

		// �������������ô�����ȴ��ͻ���
		if (FD_ISSET(_socket, &fd_reads)) {
			FD_CLR(_socket, &fd_reads);
			Accept();
			return true;
		}

		// ɾ���Ͽ����ӵĿͻ���
		for (auto client : _clients) {
			if (FD_ISSET(client->_socket, &fd_reads) && !doRecv(findClient(client->_socket))) {
				for (auto it = _clients.begin(); it != _clients.end(); it++) {
					if ((*it)->_socket == client->_socket) {
						// cout << "�ͻ���<socket = " << client->_socket << ">��������Ͽ�����" << endl;
						delete client;
						_clients.erase(it);
						break;
					}
				}
			}
		}
		return true;
	}
	return false;
}