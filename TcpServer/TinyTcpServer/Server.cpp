#include "Server.h"

// �޲ι���
Server::Server() {
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	_socket = INVALID_SOCKET;
	_msg_num = 0;
	_socket_num = 0;
	_pkg_num = 0;
	_time_1 = time(nullptr);
	_time_2 = time(nullptr);
}

// ��������
Server::~Server() {
#ifdef _WIN32
	WSACleanup();
#endif
	Close();
}

// ��ʼ��SOCKET
bool Server::initSocket() {
	if (isRun()) Close();
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (isRun()) return true;
	else return false;
}

// �󶨶˿�
bool Server::doBind(const char* ip, unsigned short port) {
	if (!isRun()) initSocket();
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
bool Server::doListen(int num) {
	int ret = listen(_socket, num);
	return ret != SOCKET_ERROR;
}

// �ȴ��ͻ���
bool Server::Accept() {
	sockaddr_in client_addr = {};
	int addr_len = sizeof(sockaddr_in);
	ClientInfo* client = new ClientInfo();
#ifdef _WIN32
	client->_socket = accept(_socket, (sockaddr*)&client_addr, &addr_len);
#else
	client->_socket = accept(_socket, (sockaddr*)&client_addr, (socklen_t*)&addr_len);
#endif
	if (client->_socket != INVALID_SOCKET) {
		// cout << "[" << ++_socket_num << "]�ͻ���<socket = " << client->_socket << ">���ӵ�������" << endl;
		_clients.push_back(client);
		return true;
	}
	else return false;
}

// ������Ϣ
bool Server::doSend(SOCKET socket, Header* header) {
	if (isRun() && header) {
		int ret = send(socket, (const char*)header, header->_length, 0);
		return ret > 0;
	}
	return false;
}

// ����ȫ����Ϣ
bool Server::doSendAll(Header* header) {
	for (auto client : _clients) doSend(client->_socket, header);
	return true;
}

// ������Ϣ
bool Server::doRecv(ClientInfo* client) {
	int len = (int)recv(client->_socket, client->_buffer + client->_data_len, BUFF_SIZE - client->_data_len, 0);
	if (len <= 0) return false;
	client->_data_len += len;
	// cout << "[" << ++_msg_num << "]�յ��ͻ���<socket = " << client->_socket << ">��Ϣ����Ϣ����Ϊ��" << len << endl;
	while (client->_data_len >= sizeof(Header)) {
		pkgNum(pkgNum() + 1);
		Header* header = (Header*)client->_buffer;
		if (client->_data_len >= header->_length) {
			client->_data_len -= header->_length;
			processMsg(client->_socket, header);
			memcpy(client->_buffer, client->_buffer + header->_length, client->_data_len);
		}
		else break;
		_time_2 = time(nullptr);
		if (_time_2 - _time_1 >= 1) {
			cout << "ǰ1���ӽ��յ���������Ϊ��" << pkgNum() << endl;
			_time_1 = _time_2;
			pkgNum(0);
		}
	}
	return true;
}

// ������Ϣ
bool Server::processMsg(SOCKET client_sock, Header* header) {
	short type = header->_type;
	return true;
}

// ����������
bool Server::Run(timeval time_val) {
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
			if (FD_ISSET(client->_socket, &fd_reads) && !doRecv(client)) {
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

// ����״̬
bool Server::isRun() {
	return _socket != INVALID_SOCKET;
}

bool Server::Close() {
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

// ���ҿͻ���
ClientInfo* Server::findClient(SOCKET sock) {
	for (auto client : _clients) {
		if (client->_socket == sock) return client;
	}
	return NULL;
}

ClientInfo* Server::findClient(long long uid) {
	for (auto client : _clients) {
		if (client->_uid == uid) return client;
	}
	return NULL;
}

int Server::pkgNum(int pkg_num) {
	if (pkg_num != -1) _pkg_num = pkg_num;
	return _pkg_num;
}