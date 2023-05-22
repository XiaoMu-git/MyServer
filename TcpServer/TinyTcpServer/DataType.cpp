#include "DataType.h"

Client::Client(SOCKET socket) {
	_socket = socket;
	_recv_buff = new char[RECV_BUFF_SIZE];
	_send_buff = new char[SEND_BUFF_SIZE];
	_recv_pos = _send_pos = 0;
}

// ·¢ËÍÏûÏ¢
void Client::doSend(Header* header) {
	const char* send_data = (const char*)header;
	int data_size = header->_size;
	while (true) {
		if (_send_pos + data_size >= SEND_BUFF_SIZE) {
			int copy_len = SEND_BUFF_SIZE - _send_pos;
			memcpy(_send_buff + _send_pos, send_data, copy_len);
			send(_socket, _send_buff, SEND_BUFF_SIZE, 0);
			send_data += copy_len;
			data_size -= copy_len;
			_send_pos = 0;
		}
		else {
			memcpy(_send_buff + _send_pos, send_data, data_size);
			_send_pos += data_size;
			break;
		}
	}
}

Client::~Client() {
	delete _recv_buff;
	delete _send_buff;
}

Header::Header() {
	_type = CMD_ERROR;
	_size = sizeof(Header);
}

Header::~Header() {

}

Message::Message() {
	msg = new char[MESSAGE_SIZE];
}

Message::~Message() {
	delete msg;
}