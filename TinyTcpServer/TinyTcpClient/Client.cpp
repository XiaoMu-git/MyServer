#include "Client.h"

EasyTcpClient::EasyTcpClient() {
	_sock = INVALID_SOCKET;
	_isConnect = false;
}

EasyTcpClient::~EasyTcpClient() {
	Close();
}

void EasyTcpClient::InitSocket() {
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif // _WIN32
	if (INVALID_SOCKET != _sock) {
		printf("<socket=%d关闭旧连接...\n", _sock);
		Close();
	}
	_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock) {
		printf("错误，建立Socket失败...\n");
	}
}

int EasyTcpClient::Connect(const char* ip, unsigned short port) {
	if (INVALID_SOCKET == _sock) {
		InitSocket();
	}
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(port);
#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
	_sin.sin_addr.s_addr = inet_addr(ip);
#endif
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret) {
		printf("<socket=%d>错误，连接服务器<%s:%d>失败...\n", _sock, ip, port);
	}
	else {
		_isConnect = true;
	}
	return ret;
}

void EasyTcpClient::Close() {
	if (_sock != INVALID_SOCKET)
	{
#ifdef _WIN32
		closesocket(_sock);
		WSACleanup();
#else
		close(_sock);
#endif
		_sock = INVALID_SOCKET;
	}
	_isConnect = false;
}

bool EasyTcpClient::OnRun() {

}

bool EasyTcpClient::isRun() {
	return _sock != INVALID_SOCKET && _isConnect;
}

int EasyTcpClient::RecvData(SOCKET cSock) {

}

void EasyTcpClient::OnNetMsg(DataHeader* header) {

}

int EasyTcpClient::SendData(DataHeader* header, int nLen) {
	int ret = SOCKET_ERROR;
	if (isRun() && header) {
		ret = send(_sock, (const char*)header, nLen, 0);
		if (SOCKET_ERROR == ret) {
			Close();
		}
	}
	return ret;
}