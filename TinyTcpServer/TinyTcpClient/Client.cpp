#include "Client.h"
#include "DataPackage.h"

Client::Client() {
	_socket = INVALID_SOCKET;
}

Client::~Client() {
	doClose();
}

// 初始化socket
bool Client::initSocket() {
	if (_socket == INVALID_SOCKET) {
#ifdef _WIN32
		/*启动 Windows socket 2.2环境*/
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat);
#endif
		_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		return _socket != INVALID_SOCKET;
	}
	return false;
}

// 连接服务器
bool Client::doConnect(const char* ip = "127.0.0.1", int post = 2976) {
	if (_socket == INVALID_SOCKET) initSocket();
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(post);
#ifdef _WIN32
	_addr.sin_addr.S_un.S_addr = inet_addr(ip);
#else
	_addr.sin_addr.s_addr = inet_addr(ip);
#endif
	int res = connect(_socket, (sockaddr*)&_addr, sizeof(_addr));
	return res != SOCKET_ERROR;
}

// 接收消息
bool Client::doRecv() {
	int res = recv(_socket, _recv, sizeof(Header), 0);
	if (res <= 0) return false;
	Header* h_temp = (Header*)_recv;
	int h_size = sizeof(Header);
	recv(_socket, _recv + h_size, h_temp->length - h_size, 0);
}

// 发送消息
bool Client::doSend() {
	int res = send(_socket, _recv, sizeof(Header), 0);
	switch (((Header*)_recv)->cmd_type) {
	case CMD_MESSAGE: {
		// 接收message消息
		recv(_socket, recv_temp + sizeof(Header), recv_header->length - sizeof(Header), 0);
		Message* recv_message = (Message*)recv_temp;
		break;
	}
	case CMD_LOGIN: {
		// 接收他人login消息
		recv(_socket, recv_temp + sizeof(Header), recv_header->length - sizeof(Header), 0);
		UserInfo* recv_user_info = (UserInfo*)recv_temp;
		break;
	}
	case CMD_LOGOUT: {
		// 接收他人logout消息
		recv(_socket, recv_temp + sizeof(Header), recv_header->length - sizeof(Header), 0);
		UserInfo* recv_user_info = (UserInfo*)recv_temp;
		break;
	}
	}
}

// 登陆服务器
bool Client::doLogin() {
	Response recv_response;
	_info.cmd_type = CMD_LOGIN;
	_info.socket = _socket;
	send(_socket, (char*)&_info, _info.length, 0);
	recv(_socket, (char*)&recv_response, recv_response.length, 0);
	if (recv_response.resp == CMD_TREU) return true;
	else return false;
}

// 开启服务
bool Client::doRun(const timeval time_val = { 1, 0 }) {
	fd_set fd_read; FD_ZERO(&fd_read); FD_SET(_socket, &fd_read);
	int res = select(_socket + 1, &fd_read, NULL, NULL, &time_val);
	if (res < 0) return false;

	if (FD_ISSET(_socket, &fd_read)) {
		FD_CLR(_socket, &fd_read);
		return dispose();
	}

	return true;
}

// 处理消息
bool Client::dispose() {


}

// 关闭socket
bool Client::doClose() {
	if (_socket != INVALID_SOCKET) {
#ifdef _WIN32
		closesocket(_socket);
		/*关闭 Windows socket 2.2环境*/
		WSACleanup();
#else
		close(_socket);
#endif
		_socket = INVALID_SOCKET;
	}
	return false;
}
