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
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_RESPOND
};

class DataHeader {
public:
	short cmd_type;
	short data_len;
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
	SOCKET client_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (client_sock == INVALID_SOCKET) cout << "Socket创建失败" << endl;
	else cout << "Socket创建成功" << endl;

	/*连接服务器*/
	sockaddr_in client_sockaddr_in;
	client_sockaddr_in.sin_family = AF_INET;
	client_sockaddr_in.sin_port = htons(4567);
	client_sockaddr_in.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (connect(client_sock, (sockaddr*)&client_sockaddr_in, sizeof(sockaddr_in)) == SOCKET_ERROR) cout << "服务器连接失败" << endl;
	else cout << "服务器连接成功" << endl;

	while (true) {
		/*向服务器发送命令*/
		char cmd[128];
		cin >> cmd;
		if (!strcmp(cmd, "exit")) break;
		else if (!strcmp(cmd, "login")) {
			Login send_login;
			Respond send_respond;
			cout << "username：", cin >> send_login.name;
			cout << "password：", cin >> send_login.password;
			send(client_sock, (char*)&send_login, send_login.data_len, 0);
			recv(client_sock, (char*)&send_respond, send_respond.data_len, 0);
			if (send_respond.respond == CMD_TRUE) cout << "登陆成功" << endl;
			else cout << "登陆失败" << endl;
		}
		else if (!strcmp(cmd, "logout")) {
			Login send_logout;
			Respond send_respond;
			cout << "username：", cin >> send_logout.name;
			send(client_sock, (char*)&send_logout, send_logout.data_len, 0);
			recv(client_sock, (char*)&send_respond, send_respond.data_len, 0);
			if (send_respond.respond == CMD_TRUE) cout << "退出成功" << endl;
			else cout << "退出失败" << endl;
		}
		else {

		}
	}

	WSACleanup();
	system("pause");
	return 0;
}