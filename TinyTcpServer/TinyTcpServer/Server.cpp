#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include <WinSock2.h>
#include <vector>
#include <algorithm>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

std::vector<SOCKET> vec_client_sockets;

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
	SYSTEMTIME sysTime = { 0 };
	GetSystemTime(&sysTime);

	char recv_temp[4096];
	if (recv(client_socket, recv_temp, sizeof(Header), 0) <= 0) {
		printf("[%d:%d:%d]客户端<Socket=%d>已退出，连接断开\n", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond, client_socket);
		return false;
	}
	Header *recv_header = (Header*)recv_temp;

	switch (recv_header->cmd_type) {
	case CMD_LOGIN: {
		recv(client_socket, recv_temp + sizeof(Header), recv_header->length - sizeof(Header), 0);
		Login* recv_login = (Login*)&recv_temp;
		Response send_response;
		printf("[%d:%d:%d]收到客户端<Socket=%d>请求：CMD_LOGIN，数据长度：%d，userName=%s PassWord=%s\n", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond, client_socket, recv_login->length, recv_login->username, recv_login->password);
		if (recv_login->username == "xiaomu" && recv_login->password == "123456") {
			send(client_socket, (char*)&send_response, send_response.length, 0);
		}
		else {
			send_response.resp = CMD_FALSE;
			send(client_socket, (char*)&send_response, send_response.length, 0);
		}
		break;
	}
	case CMD_INQUIRE: {
		recv(client_socket, recv_temp + sizeof(Header), recv_header->length - sizeof(Header), 0);
		Inquire *recv_inquire = (Inquire*)recv_temp;
		printf("[%d:%d:%d]收到客户端<Socket=%d>请求：CMD_INQUIRE，数据长度：%d，\n", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond, client_socket, recv_inquire->length);
	}
	default: {
		Response send_response;
		send_response.resp = CMD_ERROR;
		send(client_socket, (char*)&send_response, send_response.length, 0);
		break; 
	}
	}
}

int main() {
	/*启动 Windows socket 2.2环境*/
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

	/*建立服务器 socket*/
	SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == INVALID_SOCKET) printf("[%d:%d:%d]错误，建立socket失败\n");
	else printf("建立socket成功\n");

	/*绑定服务器端口*/
	sockaddr_in sock_addr_in;
	sock_addr_in.sin_family = AF_INET;
	sock_addr_in.sin_port = htons(4567);
	sock_addr_in.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(server_socket, (sockaddr*)&sock_addr_in, sizeof(sock_addr_in)) == SOCKET_ERROR) {
		printf("错误，绑定端口失败\n");
	}
	else printf("绑定端口成功\n");

	/*监听服务器端口*/
	if (listen(server_socket, 5) == SOCKET_ERROR) {
		printf("错误，端口监听失败\n");
	}
	else printf("端口监听成功\n");

	while (true) {
		SYSTEMTIME sysTime = { 0 };
		GetSystemTime(&sysTime);

		fd_set fd_read;
		FD_ZERO(&fd_read);
		FD_SET(server_socket, &fd_read);

		for (auto socket : vec_client_sockets) FD_SET(socket, &fd_read);

		/*开启 select 服务*/
		timeval time_val = { 1, 0 };
		if (select(server_socket + 1, &fd_read, NULL, NULL, &time_val) < 0) {
			printf("select服务结束\n");
			break;
		}

		if (FD_ISSET(server_socket, &fd_read)) {
			FD_CLR(server_socket, &fd_read);
			/*等待客户端连接*/
			sockaddr_in client_sockaddr_in;
			int client_sockaddr_in_len = sizeof(sockaddr_in);
			SOCKET client_socket = accept(server_socket, (sockaddr*)&client_sockaddr_in, &client_sockaddr_in_len);
			if (client_socket == INVALID_SOCKET) {
				printf("错误，收到无效客户端sock\n");
			}
			else {
				printf("[%d:%d:%d]新客户端加入<Socket=%d><IP=%s>\n", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond, client_socket, inet_ntoa(client_sockaddr_in.sin_addr));
				for (auto socket : vec_client_sockets) {
					UserJoin send_user_join;
					send_user_join.sock = client_socket;
					send(socket, (char*)&send_user_join, send_user_join.length, 0);
				}
				vec_client_sockets.push_back(client_socket);
			}
		}

		for (int i = 0; i < fd_read.fd_count; i++) {
			if (!processMessages(fd_read.fd_array[i])) {
				auto iter = find(vec_client_sockets.begin(), vec_client_sockets.end(), fd_read.fd_array[i]);
				if (iter != vec_client_sockets.end()) {
					vec_client_sockets.erase(iter);
				}
			}
		}
	}

	/*关闭所有 socket*/
	for (auto socket : vec_client_sockets) closesocket(socket);
	closesocket(server_socket);

	/*关闭 Windows socket 2.2环境*/
	WSACleanup();
	system("pause");
	return 0;
}
