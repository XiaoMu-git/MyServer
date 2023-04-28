#include "Client.h"

// �޲ι���
Client::Client() {
	_socket = INVALID_SOCKET;
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
}

// ��������
Client::~Client() {
	Close();
#ifdef _WIN32
	WSACleanup();
#endif
}

// ��ʼ��SOCKET
bool Client::initSocket() {
	if (isRun()) Close();
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (isRun()) return true;
	else return false;
}

// ���ӷ�����
bool Client::doConnect(const char* ip, unsigned short port) {
	if (!isRun()) initSocket();
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(port);
#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
	_sin.sin_addr.s_addr = inet_addr(ip);
#endif
	int ret = connect(_socket, (sockaddr*)&_sin, sizeof(sockaddr_in));
	return ret != SOCKET_ERROR;
}

// ������Ϣ
bool Client::doSend(Header* header) {
	if (isRun() && header) {
		int ret = send(_socket, (const char*)header, header->_length, 0);
		return ret > 0;
	}
	return false;
}

// ������Ϣ
bool Client::doRecv() {
	char temp[4096] = {};
	int len = (int)recv(_socket, temp, sizeof(Header), 0);
	if (len <= 0) return false;
	Header* header = (Header*)temp;
	recv(_socket, temp + sizeof(Header), header->_length - sizeof(Header), 0);
	processMsg(header);
	return true;
}

// ������Ϣ
bool Client::processMsg(Header* header) {
	switch (header->_type) {
	case CMD_USERJOIN: {
		UserInfo* info = (UserInfo*)header;
		if (_socket != info->_socket) {
			cout << "�յ��������Ϣ��CMD_USERJOIN�����ݳ��ȣ�" << info->_length << endl;
		}
		break;
	}
	case CMD_LOGIN_RESULT: {
		Response* result = (Response*)header;
		cout << "�յ��������Ϣ��CMD_LOGIN_RESULT�����ݳ��ȣ�" << result->_length << endl;
		break;
	}
	case CMD_LOGOUT_RESULT: {
		Response* result = (Response*)header;
		cout << "�յ��������Ϣ��CMD_LOGOUT_RESULT�����ݳ��ȣ�" << result->_length << endl;
		break;
	}
	case CMD_MESSAGE: {
		Message* message = (Message*)header;
		if (message->_socket != _socket) {
			cout << "<socket = " << message->_socket << ">" << message->_message << endl;
		}
		break;
	}
	default: {
		return false;
		break;
	}
	}
	return true;
}

// �ͻ�������
bool Client::Run(const timeval time_val) {
	if (isRun()) {
		fd_set fd_reads;
		FD_ZERO(&fd_reads);
		FD_SET(_socket, &fd_reads);
		int ret = select(_socket + 1, &fd_reads, 0, 0, &time_val);
		if (ret < 0) return false;
		if (FD_ISSET(_socket, &fd_reads)) {
			FD_CLR(_socket, &fd_reads);
			if (!doRecv()) return false;
		}
		return true;
	}
	cout << "�ͻ��˿�����..." << endl;
	return false;
}

// ����״̬
bool Client::isRun() {
	return _socket != INVALID_SOCKET;
}

// �رտͻ���
bool Client::Close() {
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

SOCKET Client::getSocket() {
	return _socket;
}

char* Client::getUsername() {
	return _username;
}

void Client::setUsername(const char* username) {
	strcpy(_username, username);
}

char* Client::getPassword() {
	return _password;
}

void Client::setPassword(const char* password) {
	strcpy(_password, password);
}