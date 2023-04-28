#ifndef _DataType_h_
#define _DataType_h_

enum CMD {
	CMD_ERROR,
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
};

class Response : public Header {
public:
	bool _result;
	Response() {
		_length = sizeof(Response);
		_result = false;
	}
};

class UserInfo : public Header {
public:
	char _username[32];
	char _password[32];
	SOCKET _socket;
	UserInfo() {
		_type = CMD_ERROR;
		_length = sizeof(UserInfo);
	}
};

class Message : public Header {
public:
	SOCKET _socket;
	char _message[256];
	Message() {
		_type = CMD_MESSAGE;
		_length = sizeof(Message);
	}
};

class Command : public Header {
public:
	char _command[256];
	Command() {
		_type = CMD_COMMAND;
		_length = sizeof(Command);
	}
};

#endif // !_DataType_h_
