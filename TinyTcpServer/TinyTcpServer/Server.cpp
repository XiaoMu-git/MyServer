#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include <WinSock2.h>
#include <vector>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

enum CMD {
	CMD_ERROR,
	CMD_TREU,
	CMD_FALSE,
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_RESPONSE,
	CMD_MESSAGE,
	CMD_USERINFO
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

class Response : public Header {
public:
	short resp;
	Response() {
		this->length = sizeof(Response);
		this->cmd_type = CMD_RESPONSE;
		this->resp = CMD_TREU;
	}
};

class Message : public Header {
public:
	char msg[128];
	char username[32];
	SOCKET send_socket;
	Message() {
		this->length = sizeof(Message);
		this->cmd_type = CMD_MESSAGE;
	}
};

class UserInfo : public Header {
public:
	char username[32];
	char password[32];
	SOCKET socket;
	UserInfo() {
		this->length = sizeof(UserInfo);
	}
};

std::vector<UserInfo> clients_info;
SYSTEMTIME sysTime = { 0 };

bool processMessages(SOCKET client_socket) {
	GetSystemTime(&sysTime);
	char recv_temp[4096];
	if (recv(client_socket, recv_temp, sizeof(Header), 0) <= 0) {
		for (auto client : clients_info) {
			if (client.socket == client_socket) {
				UserInfo send_user_info;
				strcpy(send_user_info.username, client.username);
				send_user_info.socket = client.socket;
				send_user_info.cmd_type = CMD_LOGOUT;
				for (auto client : clients_info) {
					send(client.socket, (char*)&send_user_info, send_user_info.length, 0);
				}
			}
		}
		printf("[%02d:%02d:%02d]", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond);
		printf("客户端<Socket=%d>已退出，连接断开\n", client_socket);
		return false;
	}
	Header* recv_header = (Header*)recv_temp;

	switch (recv_header->cmd_type) {
		case CMD_LOGIN: {
			// 处理登录请求
			recv(client_socket, recv_temp + sizeof(Header), recv_header->length - sizeof(Header), 0);
			UserInfo* recv_user_info = (UserInfo*)recv_temp;
			Response send_response;
			printf("[%02d:%02d:%02d]", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond);
			printf("收到客户端<Socket=%d>请求：CMD_LOGIN，数据长度：%d，userName=%s\n", client_socket, recv_user_info->length, recv_user_info->username);
			if (!strcmp(recv_user_info->password, "123456")) {
				send(client_socket, (char*)&send_response, send_response.length, 0);
				for (auto client : clients_info) {
					send(client.socket, (char*)recv_user_info, recv_user_info->length, 0);
				}
			}
			else {
				send_response.resp = CMD_FALSE;
				send(client_socket, (char*)&send_response, send_response.length, 0);
			}

			for (auto iter = clients_info.begin(); iter != clients_info.end(); iter++) {
				if ((*iter).socket == client_socket) {
					strcpy((*iter).username, recv_user_info->username);
					strcpy((*iter).password, recv_user_info->password);
					break;
				}
			}
			break;
		}
		case CMD_MESSAGE: {
			// 转发客户端消息
			recv(client_socket, recv_temp + sizeof(Header), recv_header->length - sizeof(Header), 0);
			Message* recv_message = (Message*)recv_temp;
			printf("[%02d:%02d:%02d]", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond);
			printf("收到客户端<Socket=%d>请求：CMD_MESSAGE，数据长度：%d\n", client_socket, recv_message->length);
			for (auto client : clients_info) {
				send(client.socket, (char*)recv_message, recv_message->length, 0);
			}
		}
	}
}

int main() {
	GetSystemTime(&sysTime);
	/*启动 Windows socket 2.2环境*/
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

	/*建立服务器 socket*/
	SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	printf("[%02d:%02d:%02d]", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond);
	if (server_socket == INVALID_SOCKET) printf("错误，建立socket失败\n");
	else printf("建立socket成功\n");

	/*绑定服务器端口*/
	sockaddr_in sock_addr_in;
	sock_addr_in.sin_family = AF_INET;
	sock_addr_in.sin_port = htons(4567);
	sock_addr_in.sin_addr.S_un.S_addr = INADDR_ANY;
	printf("[%02d:%02d:%02d]", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond);
	if (bind(server_socket, (sockaddr*)&sock_addr_in, sizeof(sock_addr_in)) == SOCKET_ERROR) {
		printf("错误，绑定端口失败\n");
	}
	else printf("绑定端口成功\n");

	/*监听服务器端口*/
	printf("[%02d:%02d:%02d]", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond);
	if (listen(server_socket, 5) == SOCKET_ERROR) {
		printf("错误，端口监听失败\n");
	}
	else printf("端口监听成功\n");

	while (true) {
		GetSystemTime(&sysTime);
		fd_set fd_read;
		FD_ZERO(&fd_read);
		FD_SET(server_socket, &fd_read);

		// 将新连接的 socket 加入 fd_read
		for (auto client : clients_info) FD_SET(client.socket, &fd_read);

		/*开启 select 服务*/
		timeval time_val = { 1, 0 };
		if (select(server_socket + 1, &fd_read, NULL, NULL, &time_val) < 0) {
			printf("[%02d:%02d:%02d]", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond);
			printf("select服务结束\n");
			break;
		}

		// 判断服务器 socket 存活
		if (FD_ISSET(server_socket, &fd_read)) {
			FD_CLR(server_socket, &fd_read);
			/*等待客户端连接*/
			sockaddr_in client_sockaddr_in;
			int client_sockaddr_in_len = sizeof(sockaddr_in);
			SOCKET client_socket = accept(server_socket, (sockaddr*)&client_sockaddr_in, &client_sockaddr_in_len);
			printf("[%02d:%02d:%02d]", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond);
			if (client_socket == INVALID_SOCKET) {
				printf("错误，收到无效客户端sock\n");
			}
			else {
				printf("新客户端加入<Socket=%d><IP=%s>\n", client_socket, inet_ntoa(client_sockaddr_in.sin_addr));
				UserInfo new_client;
				new_client.socket = client_socket;
				clients_info.push_back(new_client);
			}
		}

		// 移除容器中断连的客户端
		for (int i = 0; i < fd_read.fd_count; i++) {
			if (!processMessages(fd_read.fd_array[i])) {
				for (auto iter = clients_info.begin(); iter != clients_info.end(); iter++) {
					if ((*iter).socket == fd_read.fd_array[i]) {
						clients_info.erase(iter);
						break;
					}
				}
			}
		}
	}

	/*关闭所有 socket*/
	for (auto client : clients_info) closesocket(client.socket);
	closesocket(server_socket);

	/*关闭 Windows socket 2.2环境*/
	WSACleanup();
	system("pause");
	return 0;
}
