#include "DataType.h"

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