#include "Client.h"
#include <iostream>
using namespace std;

// 无参构造
Client::Client() {
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSAData data;
	WSAStartup(ver, &data);
#endif // _WIN32
	_socket = INVALID_SOCKET;
	_uid = -1;
	_buffer = new char[BUFF_SIZE];
	_data_len = 0;
	_connect = false;
}

// 析构函数
Client::~Client() {
#ifdef _WIN32
	WSACleanup();
#endif // _WIN32
	doClose();
}

// 初始化 socket
bool Client::doSocket() {
	if (isRun()) doClose();
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (isRun()) return true;
	else return false;
}

// 连接服务器
bool Client::doConnect(const char* ip, unsigned short port) {
	if (!isRun()) doSocket();
	else return false;
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

// 向服务器发送消息
bool Client::doSend(Header* header, int length) {
	if (isRun() && header) {
		header->_uid = _uid;
		int ret = send(_socket, (const char*)header, header->_length, 0);
		return ret > 0;
	}
	return false;
}

// 接收服务器消息
bool Client::doRecv() {
	int recv_len = (int)recv(_socket, _buffer + _data_len, BUFF_SIZE - _data_len, 0);
	if (recv_len <= 0) {
		cout << "<socket=" << _socket << ">与服务器断开连接" << endl;
		doClose();
		return false;
	}
	else {
		_data_len += recv_len;
		cout << "收到服务器消息，消息长度为：" << recv_len << endl;
		while (_data_len >= sizeof(Header)) {
			Header* header = (Header*)_buffer;
			if (_data_len >= header->_length) {
				doDispose(header);
				_data_len -= header->_length;
				memcpy(_buffer, _buffer + header->_length, _data_len);
			}
			else break;
		}
		return true;
	}
}

// 处理消息
bool Client::doDispose(Header* header) {
	short type = header->_type;
	return true;
}

// 客户端运行
bool Client::doRun(timeval time_val) {
	if (isRun()) {
		fd_set fd_reads;
		FD_ZERO(&fd_reads);
		FD_SET(_socket, &fd_reads);
		int ret = select(_socket + 1, &fd_reads, NULL, NULL, &time_val);
		if (ret < 0) return false;
		if (FD_ISSET(_socket, &fd_reads)) {
			FD_CLR(_socket, &fd_reads);
			if (!doRecv()) return false;
		}
		return true;
	}
	return false;
}

// 客户端运行状态
bool Client::isRun() {
	return _socket != INVALID_SOCKET;
}

bool Client::doClose() {
	if (isRun()) {
#ifdef _WIN32
		closesocket(_socket);
#else
		close(_socket);
#endif
		_socket = INVALID_SOCKET;
		_connect = false;
		return true;
	}
	return false;
}

// _socket 接口
SOCKET Client::Socket(const SOCKET socket) {
	if (socket != -1) _socket = socket;
	return _socket;
}

// _uid 接口
long long Client::Uid(long long uid) {
	if (uid != -1) _uid = uid;
	return _uid;
}

// _connect 接口
bool Client::Connect(int connect) {
	if (connect != -1) _connect = connect;
	return _connect;
}