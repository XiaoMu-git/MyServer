#include "Client.h"

char buffer[BUFF_SIZE] = {};

// 无参构造
Client::Client() {
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	_socket = INVALID_SOCKET;
	_data_len = 0;
	_msg_num = 0;
	_uid = -1;
	_is_connect = false;
}

// 析构函数
Client::~Client() {
	Close();
#ifdef _WIN32
	WSACleanup();
#endif
}

// 初始化SOCKET
bool Client::initSocket() {
	if (isRun()) Close();
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (isRun()) return true;
	else return false;
}

// 连接服务器
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

// 发送消息
bool Client::doSend(Header* header) {
	if (isRun() && header) {
		header->_uid = _uid;
		int ret = send(_socket, (const char*)header, header->_length, 0);
		return ret > 0;
	}
	return false;
}

// 接收消息
bool Client::doRecv() {
	int len = (int)recv(_socket, buffer + _data_len, BUFF_SIZE - _data_len, 0);
	if (len <= 0) {
		cout << "<socket=" << _socket << ">与服务器断开连接" << endl;
		Close();
		return false;
	}
	_data_len += len;
	cout << "[" << ++_msg_num << "]收到服务器消息，消息长度为：" << len << endl;
	while (_data_len >= sizeof(Header)) {
		Header* header = (Header*)buffer;
		if (_data_len >= header->_length) {
			_data_len -= header->_length;
			processMsg(header);
			memcpy(buffer, buffer + header->_length, _data_len);
		}
		else break;
	}
	return true;
}

// 处理消息
bool Client::processMsg(Header* header) {
	short type = header->_type;
	return true;
}

// 客户端启动
bool Client::Run(timeval time_val) {
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
	return false;
}

// 运行状态
bool Client::isRun() {
	return _socket != INVALID_SOCKET;
}

// 关闭客户端
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

SOCKET Client::Socket(const SOCKET socket) {
	if (socket != -1) _socket = socket;
	return _socket;
}

char* Client::Username(const char* username) {
	if (username != NULL) strcpy(_username, username);
	return _username;
}

char* Client::Password(const char* password) {
	if (password != NULL) strcpy(_password, password);
	return _password;
}

long long Client::Uid(long long uid) {
	if (uid != -1) _uid = uid;
	return _uid;
}

bool Client::isConnect(int is_connect) {
	if (is_connect != -1) _is_connect = is_connect;
	return _is_connect;
}
