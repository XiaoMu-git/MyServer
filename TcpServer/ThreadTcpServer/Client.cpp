#include "Client.h"

// 无参构造
Client::Client() {
	_recv_buff = new char[RECV_BUFF_SIZE];
	_send_buff = new char[SEND_BUFF_SIZE];
	_recv_data_len = _send_data_len = 0;
	_socket = INVALID_SOCKET;
	_uid = -1;
}

// 析构函数
Client::~Client() {
	delete _recv_buff;
	delete _send_buff;
}
