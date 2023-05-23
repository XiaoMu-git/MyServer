#include "DataType.h"

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

Task::Task() {

}

Task::~Task() {

}

SendTask::SendTask(Client* client, Header* header) {
	_client = client;
	_header = header;
}

void SendTask::doTask() {
	int send_size = _header->_size;
	const char* send_data = (const char*)_header;
	while (true) {
		if (_client->_send_pos + send_size >= SEND_BUFF_SIZE) {
			int copy_size = SEND_BUFF_SIZE - _client->_send_pos;
			memcpy(_client->_send_buff + _client->_send_pos, send_data, copy_size);
			send_data += copy_size;
			send_size -= copy_size;
			send(_client->_socket, _client->_send_buff, SEND_BUFF_SIZE, 0);
			_client->_send_pos = 0;
		}
		else {
			memcpy(_client->_send_buff + _client->_send_pos, send_data, send_size);
			_client->_send_pos += send_size;
			break;
		}
	}
}

SendTask::~SendTask() {

}

Header::Header() {
	_type = CMD_ERROR;
	_size = sizeof(Header);
}

Header::~Header() {

}

Message::Message() {
	_type = CMD_MESSAGE;
	msg = new char[MESSAGE_SIZE];
	_size = sizeof(Message) + MESSAGE_SIZE;
}

Message::~Message() {
	delete msg;
}