#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include <WinSock2.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
	WORD var = MAKEWORD(2, 2);
	WSAData dat;
	WSAStartup(var, &dat);

	/*��Socket API��������TCP�ͻ���*/
	/*1.����һ��socket*/
	SOCKET my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == my_sock) {
		cout << "Socket����ʧ�ܡ�����" << endl;
	}
	else {
		cout << "Socket�����ɹ�������" << endl;
	}

	/*2.���ӷ����� connect*/
	sockaddr_in my_sockaddr = {};
	my_sockaddr.sin_family = AF_INET;
	my_sockaddr.sin_port = htons(4567);
	my_sockaddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (connect(my_sock, (sockaddr*)&my_sockaddr, sizeof(sockaddr_in)) == SOCKET_ERROR) {
		cout << "���ӷ�����ʧ�ܡ�����" << endl;
	}
	else {
		cout << "���ӷ������ɹ�������" << endl;
	}

	while (true) {
		char msg_buf[256] = {}, rec_buf[256] = {};
		cin >> msg_buf;
		/*3.��������*/
		if (!strcmp(msg_buf, "exit")) {
			break;
		}

		/*4.�����������һ����Ϣ send*/
		send(my_sock, msg_buf, strlen(msg_buf) + 1, 0);

		/*5.���շ�������Ϣ recv*/
		recv(my_sock, rec_buf, 256, 0);
		cout << rec_buf << endl;
	}

	/*6.�ر�socket closesocket*/
	closesocket(my_sock);

	WSACleanup();
	system("pause");
	return 0;
}