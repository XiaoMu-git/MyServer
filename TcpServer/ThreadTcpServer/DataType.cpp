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


SendTask::SendTask(ClientPtr& client, HeaderPtr& header) {
	_client = client;
	_header = header;
}

void SendTask::doTask() {
	int data_len = _header->_size;
	const char* data = (const char*)_header->_size;
	while (true) {
		if (_client->_send_pos + data_len >= SEND_BUFF_SIZE) {
			int copy_len = SEND_BUFF_SIZE - _client->_send_pos;
			memcpy(_client->_send_buff + _client->_send_pos, data, copy_len);
			send(_client->_socket, _client->_send_buff, SEND_BUFF_SIZE, 0);
			data += copy_len;
			data_len -= copy_len;
			_client->_send_pos = 0;
		}
		else {
			memcpy(_client->_send_buff + _client->_send_pos, data, data_len);
			_client->_send_pos += data_len;
			break;
		}
	}
}
