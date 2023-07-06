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
	_timer.upData();
}

Client::Client(SOCKET socket) {
	_socket = socket;
	_recv_buff = new char[RECV_BUFF_SIZE];
	_send_buff = new char[SEND_BUFF_SIZE];
	_recv_pos = _send_pos = 0;
	_timer.upData();
}

void Client::doSend(HeaderPtr header) {
	int data_len = header->_size;
	const char* data = (const char*)header->_size;
	while (true) {
		if (_send_pos + data_len >= SEND_BUFF_SIZE) {
			int copy_len = SEND_BUFF_SIZE - _send_pos;
			memcpy(_send_buff + _send_pos, data, copy_len);
			send(_socket, _send_buff, SEND_BUFF_SIZE, 0);
			data += copy_len;
			data_len -= copy_len;
			_send_pos = 0;
			_timer.upData();
		}
		else {
			memcpy(_send_buff + _send_pos, data, data_len);
			_send_pos += data_len;
			break;
		}
	}
}

void Client::checkSend() {
	if (_send_pos > 0 && _timer.getMSec() >= SEND_BUFF_TIME) {
		send(_socket, (const char*)_send_buff, _send_pos, 0);
		_send_pos = 0;
		_timer.upData();
	}
}

Client::~Client() {
	delete _recv_buff;
	delete _send_buff;
}


SendTask::SendTask(ClientPtr& client, HeaderPtr& header) {
	_client = client;
	_header = header;
}

void SendTask::doTask() {
	_client->doSend(_header);
}
