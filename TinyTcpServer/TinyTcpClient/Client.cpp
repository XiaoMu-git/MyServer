#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include <WinSock2.h>
#include <vector>
#include <algorithm>
#include <thread>

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

UserInfo my_info;
SYSTEMTIME sysTime = { 0 };

void doLogin(SOCKET client_socket) {
	while (true) {
		GetSystemTime(&sysTime);
		Sleep(1000);
		system("cls");
		UserInfo send_user_info;
		Response recv_response;
		char cmd[256];
		printf("[%02d:%02d:%02d]", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond);
		printf("���ȵ�¼�˻�\n");
		printf("username: "); scanf("%s", cmd);
		strcpy(send_user_info.username, cmd);
		printf("password: "); scanf("%s", cmd);
		strcpy(send_user_info.password, cmd);
		send_user_info.cmd_type = CMD_LOGIN;
		send_user_info.socket = client_socket;
		send(client_socket, (char*)&send_user_info, send_user_info.length, 0);
		recv(client_socket, (char*)&recv_response, recv_response.length, 0);
		printf("[%02d:%02d:%02d]", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond);
		if (recv_response.resp == CMD_TREU) {
			strcpy(my_info.username, send_user_info.username);
			strcpy(my_info.password, send_user_info.password);
			printf("��½�ɹ�!\n");
			return;
		}
		else printf("�˺Ż�������������ԡ�����\n");
	}
}

void cmdThread(SOCKET client_socket) {
	while (true) {
		GetSystemTime(&sysTime);
		Message recv_message;
		recv_message.send_socket = client_socket;
		strcpy(recv_message.username, my_info.username);
		scanf("%s", recv_message.msg);
		if (recv_message.msg[0] == '/') {
			// ��������
		}
		else send(client_socket, (char*)&recv_message, recv_message.length, 0);
	}
}

bool processMessages(SOCKET client_socket) {
	GetSystemTime(&sysTime);
	char recv_temp[4096];
	if (recv(client_socket, recv_temp, sizeof(Header), 0) <= 0) {
		printf("[%02d:%02d:%02d]", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond);
		printf("��������Ͽ�����\n", client_socket);
		return false;
	}
	Header* recv_header = (Header*)recv_temp;

	switch (recv_header->cmd_type) {
		case CMD_MESSAGE: {
			recv(client_socket, recv_temp + sizeof(Header), recv_header->length - sizeof(Header), 0);
			Message* recv_message = (Message*)recv_temp;
			if (recv_message->send_socket != client_socket) {
				printf("[%02d:%02d:%02d]", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond);
				printf("%s: %s\n", recv_message->username, recv_message->msg);
			}
			break;
		}
		case CMD_LOGIN: {
			recv(client_socket, recv_temp + sizeof(Header), recv_header->length - sizeof(Header), 0);
			UserInfo* recv_user_info = (UserInfo*)recv_temp;
			if (recv_user_info->socket != client_socket) {
				printf("[%02d:%02d:%02d]", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond);
				printf("[ϵͳ��Ϣ]�û�%s����Ⱥ��\n", recv_user_info->username);
			}
			break;
		}
		case CMD_LOGOUT: {
			recv(client_socket, recv_temp + sizeof(Header), recv_header->length - sizeof(Header), 0);
			UserInfo* recv_user_info = (UserInfo*)recv_temp;
			printf("[%02d:%02d:%02d]", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond);
			printf("[ϵͳ��Ϣ]�û�%s�˳�Ⱥ��\n", recv_user_info->username);
			break;
		}
	}
}

int main() {
	GetSystemTime(&sysTime);
	/*���� Windows socket 2.2����*/
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

	/*�����ͻ��� socket*/
	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
	printf("[%02d:%02d:%02d]", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond);
	if (client_socket == INVALID_SOCKET) {
		printf("���󣬽���socketʧ�ܡ�����\n");
		system("pause");
		return 0;
	}
	else printf("����socket�ɹ�������\n");

	/*���ӷ�����*/
	sockaddr_in client_sockaddr_in;
	client_sockaddr_in.sin_family = AF_INET;
	client_sockaddr_in.sin_port = htons(4567);
	client_sockaddr_in.sin_addr.S_un.S_addr = inet_addr("59.110.170.223");
	printf("[%02d:%02d:%02d]", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond);
	if (connect(client_socket, (sockaddr*)&client_sockaddr_in, sizeof(client_sockaddr_in)) == SOCKET_ERROR) {
		printf("���󣬷���������ʧ�ܡ�����\n");
		system("pause");
		return 0;
	}
	else printf("���������ӳɹ�������\n");

	// �˻���¼
	doLogin(client_socket);

	/*�������߳��������� thread*/
	std::thread putIn(cmdThread, client_socket);

	while (true) {
		GetSystemTime(&sysTime);
		fd_set fd_read;
		FD_ZERO(&fd_read);
		FD_SET(client_socket, &fd_read);

		/*���� select ����*/
		timeval time_val = { 1, 0 };
		if (select(client_socket + 1, &fd_read, NULL, NULL, &time_val) < 0) {
			printf("[%02d:%02d:%02d]", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond);
			printf("select�������\n");
			break;
		}

		if (FD_ISSET(client_socket, &fd_read)) {
			FD_CLR(client_socket, &fd_read);
			if (!processMessages(client_socket)) {
				printf("[%02d:%02d:%02d]", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond);
				printf("select�������\n");
				break;
			}
		}
	}

	/*�ر� socket*/
	closesocket(client_socket);

	/*�ر� Windows socket 2.2����*/
	WSACleanup();
	system("pause");
	return 0;
}