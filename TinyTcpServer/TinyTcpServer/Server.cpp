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
	/*接收客户端命令*/
	char recv_temp[1024];
	if (recv(client_sock, (char*)&recv_temp, 1024, 0) <= 0) {
		cout << "客户端:" << client_sock << "断开连接" << endl;
		return false;
	}

	/*回复客户端消息*/
	if (((DataHeader*)recv_temp)->cmd_type == CMD_LOGIN) {
		cout << "收到客户端" << client_sock << "的CMD_LOGIN类命令" << endl;
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

	vector<SOCKET> client_socks;
	while (true) {
		// 创建三个fd，并初始化
		fd_set fd_read; FD_ZERO(&fd_read); FD_SET(server_sock, &fd_read);
		fd_set fd_write; FD_ZERO(&fd_write); FD_SET(server_sock, &fd_write);
		fd_set fd_except; FD_ZERO(&fd_except); FD_SET(server_sock, &fd_except);

		// 将新连接的客户端加入select
		for (int i = 0; i < client_socks.size(); i++) {
			FD_SET(client_socks[i], &fd_read);
		}

		// 启用select
		int ret = select(server_sock + 1, &fd_read, &fd_write, &fd_except, NULL);
		if (ret < 0) {
			cout << "select任务结束" << endl;
			break;
		}

		// 如果服务器的sock仍然存在
		if (FD_ISSET(server_sock, &fd_read)) {
			// 清理网络标志位
			FD_CLR(server_sock, &fd_read);
			/*4.等待接收客户端连接 accept*/
			sockaddr_in client_sockaddr_in;
			int client_sockaddr_in_len = sizeof(sockaddr_in);
			SOCKET client_sock = accept(server_sock, (sockaddr*)&client_sockaddr_in, &client_sockaddr_in_len);
			if (client_sock == INVALID_SOCKET) cout << "客户端连接失败" << endl;
			else {
				cout << "客户端:" << client_sock << "连接成功" << endl;
				client_socks.push_back(client_sock);
			}
		}

		// 清理断连客户端
		for (int i = 0; i < fd_read.fd_count; i++) {
			if (!processor(fd_read.fd_array[i])) {
				auto pos = find(client_socks.begin(), client_socks.end(), fd_read.fd_array[i]);
				if (pos != client_socks.end()) client_socks.erase(pos);
			}
		}
	}

	/*关闭socket*/
	closesocket(server_sock);
	for (int i = 0; i < client_socks.size(); i++) {
		closesocket(client_socks[i]);
	}

	WSACleanup();
	system("pause");
	return 0;
}