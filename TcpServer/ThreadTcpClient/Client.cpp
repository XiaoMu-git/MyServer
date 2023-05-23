#include "Client.h"

// 无参构造
Client::Client() {
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSAData data;
	WSAStartup(ver, &data);
#endif // _WIN32
	_socket = INVALID_SOCKET;
	_recv_buff = new char[RECV_BUFF_SIZE];
	_send_buff = new char[SEND_BUFF_SIZE];
	_recv_pos = _send_pos = 0;
	_connect = false;
}

// 初始化 socket
bool Client::initSocket() {
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	return _socket != INVALID_SOCKET;
}

// 连接服务器
bool Client::doConnect(const char* ip, unsigned short port) {
	if (!isRun()) initSocket();
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

// 发送消息
void Client::doSend(Header* header) {
	int ret = 0;
	int remainder_len = header->_size;
	while (true) {
		if (_send_pos + remainder_len >= SEND_BUFF_SIZE) {
			int empty_len = SEND_BUFF_SIZE - _send_pos;
			memcpy(_send_buff + _send_pos, header + (header->_size - remainder_len), empty_len);
			ret = send(_socket, (const char*)_send_buff, SEND_BUFF_SIZE, 0);
			remainder_len -= empty_len;
			_send_pos = 0;
		}
		else {
			memcpy(_send_buff + _send_pos, header, remainder_len);
			_send_pos += remainder_len;
			break;
		}
	}
}

// 接收消息
int Client::doRecv() {
	int recv_len = (int)recv(_socket, _recv_buff + _recv_pos, RECV_BUFF_SIZE - _recv_pos, 0);
	if (recv_len < 0) {
		printf("socket<%d>与服务器断开连接\n", _socket);
		doClose();
		return recv_len;
	}
	else {
		int left_pos = 0;
		_recv_pos += recv_len;
		printf("recv_len<%d>\n", recv_len);
		while (_recv_pos >= sizeof(Header)) {
			Header* header = (Header*)(_recv_buff + left_pos);
			if (_recv_pos >= header->_size) {
				doDispose(header);
				_recv_pos -= header->_size;
				left_pos += header->_size;
			}
			else break;
		}
		memcpy(_recv_buff, _recv_buff + left_pos, _recv_pos);
		return recv_len;
	}
}

// 处理消息
void Client::doDispose(Header* header) {
	short type = header->_type;
}

// 运行函数
void Client::doRun() {
	timeval time_val = { 0, 10 };
	if (isRun()) {
		fd_set fd_reads;
		FD_ZERO(&fd_reads);
		FD_SET(_socket, &fd_reads);

		int ret = select(_socket + 1, &fd_reads, NULL, NULL, &time_val);
		if (ret < 0) return;

		if (FD_ISSET(_socket, &fd_reads)) {
			FD_CLR(_socket, &fd_reads);
			if (doRecv() < 0) return;
		}
	}
}

// 运行状态
bool Client::isRun() {
	return _socket != INVALID_SOCKET;
}

// 关闭客户端
void Client::doClose() {
	if (isRun()) {
#ifdef _WIN32
		closesocket(_socket);
#else
		close(_socket);
#endif
		_socket = INVALID_SOCKET;
		_connect = false;
	}
}

// _socket 接口
SOCKET Client::Socket(const SOCKET socket) {
	if (socket != -1) _socket = socket;
	return _socket;
}

// _connect 接口
bool Client::Connect(int connect) {
	if (connect != -1) _connect = connect;
	return _connect;
}

Client::~Client() {
#ifdef _WIN32
	WSACleanup();
#endif // _WIN32
	doClose();
	delete _recv_buff;
	delete _send_buff;
}