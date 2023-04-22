#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include <WinSock2.h>
#include <vector>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

enum CMD {
	CMD_FALSE,
	CMD_TREU,
	CMD_LOGIN,
	CMD_INQUIRE,
	CMD_RESPONSE,
	CMD_USERJOIN,
	CMD_ERROR
};

class Header {
public:
	short length;
	short cmd_type;
	Header() {
		this->length = sizeof(Header);
		this->cmd_type = -1;
	}
};

class Login : public Header {
public:
	char username[32];
	char password[32];
	Login() {
		this->length = sizeof(Login);
		this->cmd_type = CMD_LOGIN;
	}
};

class Inquire : public Header {
public:
	char inquire[64];
	Inquire() {
		this->length = sizeof(Inquire);
		this->cmd_type = CMD_INQUIRE;
	}
};

class Response : public Header {
public:
	short resp;
	Response() {
		this->length = sizeof(Response);
		this->cmd_type = CMD_RESPONSE;
		this->resp = CMD_TREU;
	}
};

class UserJoin : public Header {
public:
	int sock;
	UserJoin() {
		this->length = sizeof(UserJoin);
		this->cmd_type = CMD_USERJOIN;
	}
};

bool processMessages(SOCKET client_socket) {
	char recv_temp[4096];
	if (recv(client_socket, recv_temp, sizeof(Header), 0) <= 0) {
		printf("与服务器断开连接\n", client_socket);
		return false;
	}
	Header* recv_header = (Header*)recv_temp;

	switch (recv_header->cmd_type) {
	case CMD_USERJOIN: {
		recv(client_socket, recv_temp + sizeof(Header), recv_header->length - sizeof(Header), 0);
		UserJoin* recv_user_join = (UserJoin*)recv_temp;
		printf("新客户端加入<Socket=%d>\n", recv_user_join->sock);
	}
	default: {
		break;
	}
	}
}

int main() {
	/*启动 Windows socket 2.2环境*/
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

	/*建立客户端 socket*/
	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET) printf("错误，建立socket失败\n");
	else printf("建立socket成功\n");

	/*连接服务器*/
	sockaddr_in client_sockaddr_in;
	client_sockaddr_in.sin_family = AF_INET;
	client_sockaddr_in.sin_port = htons(4567);
	client_sockaddr_in.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (connect(client_socket, (sockaddr*)&client_sockaddr_in, sizeof(client_sockaddr_in)) == SOCKET_ERROR) {
		printf("错误，服务器连接失败\n");
	}
	else printf("服务器连接成功\n");

	while (true) {
		fd_set fd_read;
		FD_ZERO(&fd_read);
		FD_SET(client_socket, &fd_read);

		/*开启 select 服务*/
		timeval time_val = { 1, 0 };
		if (select(client_socket + 1, &fd_read, NULL, NULL, &time_val) < 0) {
			printf("select服务结束\n");
			break;
		}

		if (FD_ISSET(client_socket, &fd_read)) {
			FD_CLR(client_socket, &fd_read);
			if (!processMessages(client_socket)) {
				printf("select服务结束\n");
				break;
			}
		}

		Inquire send_inquire;
		strcpy(send_inquire.inquire, "This is a test.");
		send(client_socket, (char*)&send_inquire, send_inquire.length, 0);
		Sleep(500);
	}

	/*关闭 socket*/
	closesocket(client_socket);

	/*关闭 Windows socket 2.2环境*/
	WSACleanup();
	system("pause");
	return 0;
}