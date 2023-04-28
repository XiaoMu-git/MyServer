#include "Server.h"

// �޲ι���
Server::Server() {
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	_socket = INVALID_SOCKET;
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
	UserInfo client = {};
	client._type = CMD_USERJOIN;
#ifdef _WIN32
	client._socket = accept(_socket, (sockaddr*)&client_addr, &addr_len);
#else
	client._socket = accept(_socket, (sockaddr*)&client_addr, (socklen_t*)&addr_len);
#endif
	if (client._socket != INVALID_SOCKET) {
		cout << "�ͻ���<socket = " << client._socket << ">���ӵ�������" << endl;
		doSendAll((Header*)&client);
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
	for (auto client : _clients) {
		if (!doSend(client._socket, header)) {
			return false;
		}
	}
	return true;
}

// ������Ϣ
bool Server::doRecv(SOCKET client_sock) {
	char temp[4096] = {};
	int len = (int)recv(client_sock, temp, sizeof(Header), 0);
	if (len <= 0) return false;
	Header* header = (Header*)temp;
	recv(client_sock, temp + sizeof(Header), header->_length - sizeof(Header), 0);
	processMsg(client_sock, header);
	return true;
}

// ������Ϣ
bool Server::processMsg(SOCKET client_sock, Header* header) {
	switch (header->_type) {
	case CMD_LOGIN: {
		cout << "�յ��ͻ���<socket = " << client_sock << ">��CMD_LOGIN��Ϣ" << endl;
		UserInfo* info = (UserInfo*)header;
		Response res = {};
		res._type = CMD_LOGIN_RESULT;
		res._result = true;
		doSend(client_sock, (Header*)&res);
		break;
	}
	case CMD_LOGOUT: {
		cout << "�յ��ͻ���<socket = " << client_sock << ">��CMD_LOGOUT��Ϣ" << endl;
		UserInfo* info = (UserInfo*)header;
		Response res = {};
		res._type = CMD_LOGOUT_RESULT;
		res._result = true;
		doSend(client_sock, (Header*)&res);
		break;
	}
	case CMD_MESSAGE: {
		cout << "�յ��ͻ���<socket = " << client_sock << ">��CMD_MESSAGE��Ϣ" << endl;
		doSendAll(header);
		break;
	}
	default:
		break;
	}
	return true;
}

// ����������
bool Server::Run(const timeval time_val) {
	if (isRun()) {
		fd_set fd_reads;
		FD_ZERO(&fd_reads);
		FD_SET(_socket, &fd_reads);

		// ���������ӵĿͻ���
		SOCKET max_socket = _socket;
		for (auto client : _clients) {
			FD_SET(client._socket, &fd_reads);
			max_socket = max(max_socket, client._socket);
		}
		int ret = select(max_socket + 1, &fd_reads, NULL, NULL, &time_val);
		if (ret < 0) return false;

		// �������������ô�����ȴ��ͻ���
		if (FD_ISSET(_socket, &fd_reads)) {
			FD_CLR(_socket, &fd_reads);
			Accept();
		}

		// ɾ���Ͽ����ӵĿͻ���
		for (auto client : _clients) {
			if (FD_ISSET(client._socket, &fd_reads) && !doRecv(client._socket)) {
				for (auto it = _clients.begin(); it != _clients.end(); it++) {
					if ((*it)._socket == client._socket) {
						cout << "�ͻ���<socket = " << client._socket << ">��������Ͽ�����" << endl;
						_clients.erase(it);
						break;
					}
				}
			}
		}
		return true;
	}
	cout << "������������..." << endl;
	return false;
}

// ����״̬
bool Server::isRun() {
	return _socket != INVALID_SOCKET;
}

// �رտͻ���
bool Server::Close() {
	if (isRun()) {
#ifdef _WIN32
		for (auto client : _clients) closesocket(client._socket);
		closesocket(_socket);
#else
		for (auto client : _clients) close(client._socket);
		close(_socket);
#endif
		_socket = INVALID_SOCKET;
		return true;
	}
	return false;
}