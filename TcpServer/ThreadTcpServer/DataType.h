#ifndef _DataType_h_
#define _DataType_h_

#ifdef _WIN32
#define FD_SETSIZE 1024
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<windows.h>
#include<WinSock2.h>
#include <ctime>
#pragma comment(lib,"ws2_32.lib")
#else
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#endif

#define RECV_BUFF_SIZE 10240
#define SEND_BUFF_SIZE 10240
#include <functional>
#include <vector>
#include <atomic>
#include <thread>
#include <memory>
#include <chrono>
#include <mutex>
#include <list>

enum CMD {
	CMD_ERROR,
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT
};

class Header {
public:
	short _type;
	short _size;

	Header();
};

class UserInfo : public Header {
public:
	char userName[32];
	char passWord[32];

	UserInfo();
};

class Response : public Header {
public:
	short _result;
	char _data[128];

	Response();
};

class Client {
public:
	SOCKET _socket;
	char* _recv_buff, * _send_buff;
	int _recv_pos, _send_pos;

	Client();

	Client(SOCKET socket);

	~Client();
};

typedef std::shared_ptr<Client> ClientPtr;
typedef std::shared_ptr<Header> HeaderPtr;
class Task {
public:
	virtual void doTask() = 0;
};

class SendTask : public Task {
private:
	ClientPtr _client;
	HeaderPtr _header;

public:
	SendTask(ClientPtr& client, HeaderPtr& header);

	void doTask() override;
};

#endif // !_DataType_h_
