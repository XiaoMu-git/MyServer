#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include <WinSock2.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

enum CMD {
	CMD_ERROR,
	CMD_TRUE,
	CMD_FALSE,
	CMD_COMMAND,
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_RESPOND
};

class DataHeader {
public:
	short cmd_type;
	short data_len;
};

class Command : public DataHeader {
public:
	char cmd[64];
	Command() {
		this->cmd_type = CMD_COMMAND;
		this->data_len = sizeof(Command);
	}
};

class Respond : public DataHeader {
public:
	short respond;
	Respond() {
		this->cmd_type = CMD_RESPOND;
		this->data_len = sizeof(Respond);
	}
};

class Login : public DataHeader {
public:
	char name[32];
	char password[32];
	Login() {
		this->cmd_type = CMD_LOGIN;
		this->data_len = sizeof(Login);
	}
};

class Logout : public DataHeader {
public:
	char name[32];
	Logout() {
		this->cmd_type = CMD_LOGOUT;
		this->data_len = sizeof(Logout);
	}
};

int main() {
	WORD var = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(var, &dat);

	/*建立一个socket*/
	SOCKET server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == server_sock) cout << "socket创建失败" << endl;
	else cout << "socket创建成功" << endl;

	/*绑定接收客户端连接的端口*/
	sockaddr_in server_sock_in;
	server_sock_in.sin_family = AF_INET;
	server_sock_in.sin_port = htons(4567);
	server_sock_in.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(server_sock, (sockaddr*)&server_sock_in, sizeof(sockaddr_in)) == SOCKET_ERROR) cout << "端口绑定失败" << endl;
	else cout << "端口绑定成功" << endl;

	/*监听网络端口*/
	if (listen(server_sock, 5) == SOCKET_ERROR) cout << "端口监听失败" << endl;
	else cout << "端口监听成功" << endl;

	/*等待接收客户端连接*/
	sockaddr_in client_sockaddr_in;
	int client_sockaddr_in_len = sizeof(sockaddr_in);
	SOCKET client_sock = accept(server_sock, (sockaddr*)&client_sockaddr_in, &client_sockaddr_in_len);
	if (client_sock == INVALID_SOCKET) cout << "客户端连接失败" << endl;
	else cout << "客户端连接成功" << endl;

	while (true) {
		/*接收客户端命令*/
		Command recv_cmd;
		int recv_len = recv(client_sock, (char*)&recv_cmd, sizeof(Command), 0);
		if (recv_len <= 0) break;
		else if (recv_cmd.cmd_type != CMD_COMMAND) continue;
		cout << "收到客户端的命令：" << recv_cmd.cmd << endl;

		/*回复客户端消息*/
		if (!strcmp(recv_cmd.cmd, "login")) {
			Login recv_login;
			Respond send_respond;
			send_respond.respond = CMD_FALSE;
			recv(client_sock, (char*)&recv_login, recv_login.data_len, 0);
			if (!strcmp(recv_login.name, "xiaomu") && !strcmp(recv_login.password, "123456")) {
				send_respond.respond = CMD_TRUE;
				send(client_sock, (char*)&send_respond, send_respond.data_len, 0);
			}
			else send(client_sock, (char*)&send_respond, send_respond.data_len, 0);
		}
		else if (!strcmp(recv_cmd.cmd, "logout")) {
			Logout recv_logout;
			Respond send_respond;
			send_respond.respond = CMD_FALSE;
			recv(client_sock, (char*)&recv_logout, recv_logout.data_len, 0);
			if (!strcmp(recv_logout.name, "xiaomu")) {
				send_respond.respond = CMD_TRUE;
				send(client_sock, (char*)&send_respond, send_respond.data_len, 0);
			}
			else send(client_sock, (char*)&send_respond, send_respond.data_len, 0);
		}
		else {

		}
	}

	cout << "客户端以断开链接" << endl;
	/*关闭socket*/
	closesocket(client_sock);
	closesocket(server_sock);

	WSACleanup();
	system("pause");
	return 0;
}