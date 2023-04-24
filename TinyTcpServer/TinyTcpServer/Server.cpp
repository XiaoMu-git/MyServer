#include "DataPackage.h"
#include "Server.h"

Server::Server() {
	/*启动 Windows socket 2.2环境*/
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
}

bool Server::InitSocket() {
	this->_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	return this->_socket != INVALID_SOCKET;
}

bool Server::toBind() {
	this->_addr;
	this->_addr.sin_family = AF_INET;
	this->_addr.sin_port = htons(4567);
	this->_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	return bind(this->_socket, (sockaddr*)&this->_addr, sizeof(this->_addr)) != SOCKET_ERROR;
}

bool Server::toListen() {
	return listen(this->_socket, 5) != SOCKET_ERROR;
}

bool Server::closeSocket() {
	closesocket(this->_socket);
	return true;
}

bool Server::disposeReq() {
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

Server::~Server() {
	WSACleanup();
}