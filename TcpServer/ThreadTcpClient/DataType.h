#ifndef _DATATYPE_H_
#define _DATATYPE_H_
#define MESSAGE_SIZE 256

enum CMD {
	CMD_ERROR,
	CMD_MESSAGE,
	CMD_NULL
};

class Header {
public:
	short _type;
	short _length;
	long long _uid;
	Header() {
		_length = sizeof(Header);
		_type = CMD_NULL;
		long long _uid = -1;
	}
};

class Message : public Header {
public:
	char* _message;
	Message() {
		_message = new char[MESSAGE_SIZE];
		_type = CMD_MESSAGE;
		_length = sizeof(Message);
		_uid = -1;
	}
};

#endif // !_DATATYPE_H_
