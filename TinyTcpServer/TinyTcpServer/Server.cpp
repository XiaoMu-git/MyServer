#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include <WinSock2.h>
#include <iostream>
#include <vector>

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

bool processor(SOCKET client_sock) {
	/*���տͻ�������*/
	char recv_temp[1024];
	if (recv(client_sock, (char*)&recv_temp, 1024, 0) <= 0) {
		cout << "�ͻ���:" << client_sock << "�Ͽ�����" << endl;
		return false;
	}

	/*�ظ��ͻ�����Ϣ*/
	if (((DataHeader*)recv_temp)->cmd_type == CMD_LOGIN) {
		cout << "�յ��ͻ���" << client_sock << "��CMD_LOGIN������" << endl;
		Login* recv_login = (Login*)recv_temp;
		Respond send_respond;
		send_respond.respond = CMD_FALSE;
		if (!strcmp(recv_login->name, "xiaomu") && !strcmp(recv_login->password, "123456")) {
			send_respond.respond = CMD_TRUE;
			send(client_sock, (char*)&send_respond, send_respond.data_len, 0);
		}
		else send(client_sock, (char*)&send_respond, send_respond.data_len, 0);
	}

	else if (((DataHeader*)recv_temp)->cmd_type == CMD_LOGOUT) {
		Logout* recv_logout = (Logout*)recv_temp;
		Respond send_respond;
		send_respond.respond = CMD_FALSE;
		if (!strcmp(recv_logout->name, "xiaomu")) {
			send_respond.respond = CMD_TRUE;
			send(client_sock, (char*)&send_respond, send_respond.data_len, 0);
		}
		else send(client_sock, (char*)&send_respond, send_respond.data_len, 0);
	}
	return true;
}

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

	vector<SOCKET> client_socks;
	while (true) {
		// ��������fd������ʼ��
		fd_set fd_read; FD_ZERO(&fd_read); FD_SET(server_sock, &fd_read);
		fd_set fd_write; FD_ZERO(&fd_write); FD_SET(server_sock, &fd_write);
		fd_set fd_except; FD_ZERO(&fd_except); FD_SET(server_sock, &fd_except);

		// �������ӵĿͻ��˼���select
		for (int i = 0; i < client_socks.size(); i++) {
			FD_SET(client_socks[i], &fd_read);
		}

		// ����select
		int ret = select(server_sock + 1, &fd_read, &fd_write, &fd_except, NULL);
		if (ret < 0) {
			cout << "select�������" << endl;
			break;
		}

		// �����������sock��Ȼ����
		if (FD_ISSET(server_sock, &fd_read)) {
			// ���������־λ
			FD_CLR(server_sock, &fd_read);
			/*4.�ȴ����տͻ������� accept*/
			sockaddr_in client_sockaddr_in;
			int client_sockaddr_in_len = sizeof(sockaddr_in);
			SOCKET client_sock = accept(server_sock, (sockaddr*)&client_sockaddr_in, &client_sockaddr_in_len);
			if (client_sock == INVALID_SOCKET) cout << "�ͻ�������ʧ��" << endl;
			else {
				cout << "�ͻ���:" << client_sock << "���ӳɹ�" << endl;
				client_socks.push_back(client_sock);
			}
		}

		// ��������ͻ���
		for (int i = 0; i < fd_read.fd_count; i++) {
			if (!processor(fd_read.fd_array[i])) {
				auto pos = find(client_socks.begin(), client_socks.end(), fd_read.fd_array[i]);
				if (pos != client_socks.end()) client_socks.erase(pos);
			}
		}
	}

	/*�ر�socket*/
	closesocket(server_sock);
	for (int i = 0; i < client_socks.size(); i++) {
		closesocket(client_socks[i]);
	}

	WSACleanup();
	system("pause");
	return 0;
}