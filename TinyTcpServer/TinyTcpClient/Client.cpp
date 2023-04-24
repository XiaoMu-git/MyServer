#include "Client.h"
#include "DataPackage.h"

Client::Client() {
	_socket = INVALID_SOCKET;
}

Client::~Client() {
	doClose();
}

// ��ʼ��socket
bool Client::initSocket() {
	if (_socket == INVALID_SOCKET) {
#ifdef _WIN32
		/*���� Windows socket 2.2����*/
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat);
#endif
		_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		return _socket != INVALID_SOCKET;
	}
	return false;
}

// ���ӷ�����
bool Client::doConnect(const char* ip = "127.0.0.1", int post = 2976) {
	if (_socket == INVALID_SOCKET) initSocket();
	_sockaddr.sin_family = AF_INET;
	_sockaddr.sin_port = htons(post);
#ifdef _WIN32
	_sockaddr.sin_addr.S_un.S_addr = inet_addr(ip);
#else
	_sockaddr.sin_addr.s_addr = inet_addr(ip);
#endif
	int res = connect(_socket, (sockaddr*)&_sockaddr, sizeof(_sockaddr));
	return res != SOCKET_ERROR;
}

// �ر�socket
bool Client::doClose() {
	if (_socket != INVALID_SOCKET) {
#ifdef _WIN32
		closesocket(_socket);
		/*�ر� Windows socket 2.2����*/
		WSACleanup();
#else
		close(_socket);
#endif
		_socket = INVALID_SOCKET;
	}
	return false;
}
