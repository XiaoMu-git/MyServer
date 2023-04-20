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

	/*����һ��socket*/
	SOCKET server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == server_sock) cout << "socket����ʧ��" << endl;
	else cout << "socket�����ɹ�" << endl;

	/*�󶨽��տͻ������ӵĶ˿�*/
	sockaddr_in server_sock_in;
	server_sock_in.sin_family = AF_INET;
	server_sock_in.sin_port = htons(4567);
	server_sock_in.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(server_sock, (sockaddr*)&server_sock_in, sizeof(sockaddr_in)) == SOCKET_ERROR) cout << "�˿ڰ�ʧ��" << endl;
	else cout << "�˿ڰ󶨳ɹ�" << endl;

	/*��������˿�*/
	if (listen(server_sock, 5) == SOCKET_ERROR) cout << "�˿ڼ���ʧ��" << endl;
	else cout << "�˿ڼ����ɹ�" << endl;

	/*�ȴ����տͻ�������*/
	sockaddr_in client_sockaddr_in;
	int client_sockaddr_in_len = sizeof(sockaddr_in);
	SOCKET client_sock = accept(server_sock, (sockaddr*)&client_sockaddr_in, &client_sockaddr_in_len);
	if (client_sock == INVALID_SOCKET) cout << "�ͻ�������ʧ��" << endl;
	else cout << "�ͻ������ӳɹ�" << endl;

	while (true) {
		/*���տͻ�������*/
		Command recv_cmd;
		int recv_len = recv(client_sock, (char*)&recv_cmd, sizeof(Command), 0);
		if (recv_len <= 0) break;
		else if (recv_cmd.cmd_type != CMD_COMMAND) continue;
		cout << "�յ��ͻ��˵����" << recv_cmd.cmd << endl;

		/*�ظ��ͻ�����Ϣ*/
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

	cout << "�ͻ����ԶϿ�����" << endl;
	/*�ر�socket*/
	closesocket(client_sock);
	closesocket(server_sock);

	WSACleanup();
	system("pause");
	return 0;
}