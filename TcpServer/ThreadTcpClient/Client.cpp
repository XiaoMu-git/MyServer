#include "Client.h"

Client::Client(const char* ip, unsigned short port) {
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSAData data;
	WSAStartup(ver, &data);
#endif // _WIN32
	_ip = ip;
	_port = port;
	_socket = INVALID_SOCKET;
	_recv_buff = new char[RECV_BUFF_SIZE];
	_send_buff = new char[SEND_BUFF_SIZE];
	_recv_pos = _send_pos = 0;
	_recv_num = _recv_pkg = 0;
	_send_num = _send_pkg = 0;
}

// 初始化socket
bool Client::initSocket() {
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	return _socket != INVALID_SOCKET;
}

// 连接服务器
bool Client::doConnect() {
	sockaddr_in addr_in = {};
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(_port);
#ifdef _WIN32
	addr_in.sin_addr.S_un.S_addr = inet_addr(_ip);
#else
	addr_in.sin_addr.s_addr = inet_addr(_ip);
#endif
	int ret = connect(_socket, (sockaddr*)&addr_in, sizeof(sockaddr_in));
	return ret != SOCKET_ERROR;
}

// 接收消息
int Client::doRecv() {
	int recv_len = (int)recv(_socket, _recv_buff + _recv_pos, RECV_BUFF_SIZE - _recv_pos, 0);
	if (recv_len < 0) doClose();
	else {
		int left_pos = 0;
		_recv_pos += recv_len;
		_recv_num++;
		while (_recv_pos >= sizeof(Header)) {
			Header* header = (Header*)(_recv_buff + left_pos);
			if (_recv_pos >= header->_size) {
				doDispose(header);
				_recv_pos -= header->_size;
				left_pos += header->_size;
				_recv_pkg++;
			}
			else break;
		}
		memcpy(_recv_buff, _recv_buff + left_pos, _recv_pos);
	}
	return recv_len;
}

// 发送消息
void Client::doSend(Header* header) {
	int remainder_len = header->_size;
	_send_pkg++;
	while (true) {
		if (_send_pos + remainder_len >= SEND_BUFF_SIZE) {
			int empty_len = SEND_BUFF_SIZE - _send_pos;
			memcpy(_send_buff + _send_pos, header + (header->_size - remainder_len), empty_len);
			send(_socket, (const char*)_send_buff, SEND_BUFF_SIZE, 0);
			remainder_len -= empty_len;
			_send_pos = 0;
			_send_num++;
		}
		else {
			memcpy(_send_buff + _send_pos, header, remainder_len);
			_send_pos += remainder_len;
			break;
		}
	}
}

// 处理消息
void Client::doDispose(Header* header) {

}

// 运行函数
bool Client::doRun(Header* header) {
	timeval time_val = { 0,0 };
	doSend(header);
	if (isRun()) {
		fd_set fd_reads;
		FD_ZERO(&fd_reads);
		FD_SET(_socket, &fd_reads);

		int ret = select(_socket + 1, &fd_reads, NULL, NULL, &time_val);
		if (ret < 0) return false;

		if (FD_ISSET(_socket, &fd_reads)) {
			FD_CLR(_socket, &fd_reads);
			if (doRecv() < 0) return false;
		}
		return true;
	}
	return false;
}

// 启动客户端
void Client::Start() {
	if (!initSocket()) {
		printf("socket初始化失败...\n");
		return;
	}
	if (!doConnect()) {
		printf("socket<%d>连接服务器失败...\n", _socket);
		return;
	}
}

// 运行状态
bool Client::isRun() {
	return _socket != INVALID_SOCKET;
}

// 关闭客户端
void Client::doClose() {
	delete _recv_buff;
	delete _send_buff;
	_socket = INVALID_SOCKET;
}

Client::~Client() {
	if (isRun()) {
#ifdef _WIN32
		WSACleanup();
#endif // _WIN32
		doClose();
	}
}