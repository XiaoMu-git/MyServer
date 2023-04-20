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

	/*1.建立一个socket*/
	SOCKET server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == server_sock) cout << "socket创建失败" << endl;
	else cout << "socket创建成功" << endl;

	/*2.绑定接收客户端连接的端口 bind*/
	sockaddr_in server_sock_in;
	server_sock_in.sin_family = AF_INET;
	server_sock_in.sin_port = htons(4567);
	server_sock_in.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(server_sock, (sockaddr*)&server_sock_in, sizeof(sockaddr_in)) == SOCKET_ERROR) cout << "端口绑定失败" << endl;
	else cout << "端口绑定成功" << endl;

	/*3.监听网络端口 listen*/
	if (listen(server_sock, 5) == SOCKET_ERROR) cout << "端口监听失败" << endl;
	else cout << "端口监听成功" << endl;

	/*4.等待接收客户端连接 accept*/
	sockaddr_in client_sockaddr_in;
	int client_sockaddr_in_len = sizeof(sockaddr_in);
	SOCKET client_sock = accept(server_sock, (sockaddr*)&client_sockaddr_in, &client_sockaddr_in_len);
	if (client_sock == INVALID_SOCKET) cout << "客户端连接失败" << endl;
	else cout << "客户端连接成功" << endl;

	while (true) {
		/*5.接收客户端消息 accept*/
		DataHeader recv_header;
		int recv_len = recv(client_sock, (char*)&recv_header, sizeof(DataHeader), 0);
		if (recv_len <= 0) {
			cout << "客户端以断开链接" << endl;
			break;
		}

		cout << "收到命令类型：" << recv_header.cmd_type << "，数据长度：" << recv_len << endl;

		/*回复客户端消息*/
		DataHeader send_header;
		switch (recv_header.cmd_type) {
		case CMD_LOGIN:
			LoginInfo login_info;
			LoginResult login_result;
			send_header.cmd_type = CMD_LOGIN;
			send_header.data_len = 0;
			recv(client_sock, (char*)&login_info, sizeof(LoginInfo), 0);
			cout << "用户名：" << login_info.user_name << "，密码：" << login_info.user_password << endl;
			if (login_info.user_name == "xiaomu" && login_info.user_password == "123456") login_result.result = CMD_TRUE;
			else login_result.result = CMD_FALSE;
			send(client_sock, (char*)&send_header, sizeof(DataHeader), 0);
			send(client_sock, (char*)&login_result, sizeof(LoginResult), 0);
			break;

		case CMD_LOGOUT:
			LogoutInfo logout_info;
			LogoutResult logout_result;
			send_header.cmd_type = CMD_LOGOUT;
			send_header.data_len = 0;
			recv(client_sock, (char*)&logout_info, sizeof(LogoutInfo), 0);
			if (logout_info.user_name == "xiaomu") logout_result.result = CMD_TRUE;
			else logout_result.result = CMD_FALSE;
			send(client_sock, (char*)&send_header, sizeof(DataHeader), 0);
			send(client_sock, (char*)&logout_result, sizeof(LogoutResult), 0);
			break;

		default:
			send_header.cmd_type = CMD_ERROR;
			send_header.data_len = 0;
			send(client_sock, (char*)&send_header, sizeof(DataHeader), 0);
			break;
		}
	}

	/*关闭socket*/
	closesocket(client_sock);
	closesocket(server_sock);

	WSACleanup();
	system("pause");
	return 0;
}