#include "Client.h"

// 无参构造
Client::Client() {
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSAData data;
	WSAStartup(ver, &data);
#endif // _WIN32
	_socket = INVALID_SOCKET;
	_uid = -1;
	_recv_buff = new char[RECV_BUFF_SIZE];
	_send_buff = new char[SEND_BUFF_SIZE];
	_recv_data_len = 0;
	_send_data_len = 0;
	_connect = false;
}

// 析构函数
Client::~Client() {
#ifdef _WIN32
	WSACleanup();
#endif // _WIN32
	doClose();
	delete _recv_buff;
	delete _send_buff;
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
bool Client::doSend(Header* header) {
	int ret = 0;
	int remainder_len = header->_length;
	while (true) {
		if (_send_data_len + remainder_len >= SEND_BUFF_SIZE) {
			int empty_len = SEND_BUFF_SIZE - _send_data_len;
			memcpy(_send_buff + _send_data_len, header + (header->_length - remainder_len), empty_len);
			ret = send(_socket, (const char*)_send_buff, SEND_BUFF_SIZE, 0);
			remainder_len -= empty_len;
			_send_data_len = 0;
		}
		else {
			memcpy(_send_buff + _send_data_len, header, remainder_len);
			_send_data_len += remainder_len;
			break;
		}
	}
	return ret > 0;
}

// 接收服务器消息
bool Client::doRecv() {
	int recv_len = (int)recv(_socket, _recv_buff + _recv_data_len, RECV_BUFF_SIZE - _recv_data_len, 0);
	if (recv_len <= 0) {
		printf("socket<%d>与服务器断开连接\n", _socket);
		doClose();
		return false;
	}
	else {
		int left_pos = 0;
		_recv_data_len += recv_len;
		printf("recv_len<%d>\n", recv_len);
		while (_recv_data_len >= sizeof(Header)) {
			Header* header = (Header*)(_recv_buff + left_pos);
			if (_recv_data_len >= header->_length) {
				doDispose(header);
				_recv_data_len -= header->_length;
				left_pos += header->_length;
			}
			else break;
		}
		memcpy(_recv_buff, _recv_buff + left_pos, _recv_data_len);
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