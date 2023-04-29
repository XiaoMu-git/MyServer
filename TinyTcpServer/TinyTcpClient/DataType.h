#ifndef _DataType_h_
#define _DataType_h_

#ifndef BUFF_SIZE
#define BUFF_SIZE 51200
#endif

enum CMD {
	CMD_ERROR,
	CMD_UID,
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_USERJOIN,
	CMD_MESSAGE,
	CMD_COMMAND
};

class Header {
public:
	short _type;
	short _length;
	long long _uid;
};

class Response : public Header {
public:
	bool _result;
	Response() {
		_length = sizeof(Response);
		_result = false;
		_uid = -1;
	}
};

class UserInfo : public Header {
public:
	char _username[32];
	char _password[32];
	UserInfo() {
		_type = CMD_ERROR;
		_length = sizeof(UserInfo);
		_uid = -1;
	}
};

class Message : public Header {
public:
	char _message[256];
	Message() {
		_type = CMD_MESSAGE;
		_length = sizeof(Message);
		_uid = -1;
	}
};

class Command : public Header {
public:
	char _command[256];
	Command() {
		_type = CMD_COMMAND;
		_length = sizeof(Command);
		_uid = -1;
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
