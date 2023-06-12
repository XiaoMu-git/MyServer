#include "DataType.h"

Header::Header() {
	_type = CMD_ERROR;
	_size = sizeof(Header);
}

UserInfo::UserInfo() {
	_size = sizeof(UserInfo);
}

Response::Response() {
	_result = false;
	_size = sizeof(Response);
}

Client::Client() {
	_socket = INVALID_SOCKET;
	_recv_buff = new char[RECV_BUFF_SIZE];
	_send_buff = new char[SEND_BUFF_SIZE];
	_recv_pos = _send_pos = 0;
}

Client::Client(SOCKET socket) {
	_socket = socket;
	_recv_buff = new char[RECV_BUFF_SIZE];
	_send_buff = new char[SEND_BUFF_SIZE];
	_recv_pos = _send_pos = 0;
}

Client::~Client() {
	delete _recv_buff;
	delete _send_buff;
}