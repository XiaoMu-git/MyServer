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
	CMD_LOGOUT
};

struct DataHeader {
	short cmd_type;
	short data_len;
};

struct LoginInfo {
	char user_name[32];
	char user_password[32];
};

struct LogoutInfo {
	char user_name[32];
};

struct LoginResult {
	int result;
};

struct LogoutResult {
	int result;
};

int main() {
	WORD var = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(var, &dat);

	/*1.����һ��socket*/
	SOCKET client_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (client_sock == INVALID_SOCKET) cout << "Socket����ʧ��" << endl;
	else cout << "Socket����ʧ��" << endl;

	/*2.���ӷ�����*/
	sockaddr_in client_sockaddr_in;
	client_sockaddr_in.sin_family = AF_INET;
	client_sockaddr_in.sin_port = htons(4567);
	client_sockaddr_in.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (connect(client_sock, (sockaddr*)&client_sockaddr_in, sizeof(sockaddr_in)) == SOCKET_ERROR) cout << "����������ʧ��" << endl;
	else cout << "���������ӳɹ�" << endl;

	while (true) {

		/*3.�������������Ϣ*/
		char msg_buf[128];
		cin >> msg_buf;
		DataHeader send_header;
		DataHeader recv_header;
		send_header.data_len = 0;
		if (!strcmp(msg_buf, "exit")) break;
		else if (!strcmp(msg_buf, "login")) {
			LoginResult login_result;
			LoginInfo login_info = { "xiaomu", "123456" };
			send_header.cmd_type = CMD_LOGIN;
			send(client_sock, (char*)&send_header, sizeof(DataHeader), 0);
			send(client_sock, (char*)&login_info, sizeof(LoginInfo), 0);

			/*4.���շ�������Ϣ*/
			recv(client_sock, (char*)&recv_header, sizeof(DataHeader), 0);
			recv(client_sock, (char*)&login_result, sizeof(LoginResult), 0);

			cout << "����Ľ���ǣ�" << login_result.result << endl;
		}
		else if (!strcmp(msg_buf, "logout")) {
			LogoutResult logout_result;
			LogoutInfo logout_info = { "xiaomu" };
			send_header.cmd_type = CMD_LOGOUT;
			send(client_sock, (char*)&send_header, sizeof(DataHeader), 0);
			send(client_sock, (char*)&logout_info, sizeof(LogoutInfo), 0);

			/*4.���շ�������Ϣ*/
			recv(client_sock, (char*)&recv_header, sizeof(DataHeader), 0);
			recv(client_sock, (char*)&logout_result, sizeof(LoginResult), 0);

			cout << "����Ľ���ǣ�" << logout_result.result << endl;
		}
		else cout << "�������������������������" << endl;
	}

	WSACleanup();
	system("pause");
	return 0;
}