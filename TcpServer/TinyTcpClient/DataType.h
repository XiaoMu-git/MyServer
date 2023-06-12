#ifndef _DataType_h_
#define _DataType_h_

#define BUFF_SIZE 51200
#define MESSAGE_SIZE 256

enum CMD {
	CMD_ERROR,
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_MESSAGE
};

class Header {
public:
	short _type, _size;

public:
	Header() {
		_type = CMD_ERROR;
		_size = sizeof(Header);
	}

};

class Response : public Header {
public:
	bool _result;

public:
	Response() {
		_size = sizeof(Response);
		_result = false;
	}

};

class UserInfo : public Header {
public:
	char _username[32];
	char _password[32];

public:
	UserInfo() {
		_size = sizeof(UserInfo);
	}

};

class Message : public Header {
public:
	char _msg[MESSAGE_SIZE];

public:
	Message() {
		_size = sizeof(Message);
		_type = CMD_MESSAGE;
	}

};

class ClientInfo {
public:
	SOCKET _socket;
	long long _uid;
	char _buffer[BUFF_SIZE];
	int _data_len;
	ClientInfo() {
		_socket = INVALID_SOCKET;
		_data_len = 0;
		_uid = -1;
	}
};

#endif // !_DataType_h_
