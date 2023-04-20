#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include <WinSock2.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
	WORD var = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(var, &dat);

	/*��Socket API��������TCP����ˣ���ʵ����Ϣ����ش�*/
	/*1.����һ��socket*/
	// ʹ�õ�Э��أ�AF_INET��socket���ͣ�SOCK_STREAM��Э������ࣺIPPROTO_TCP
	SOCKET my_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == my_sock) {
		cout << "socket����ʧ�ܡ�����" << endl;
	}
	else {
		cout << "socket�����ɹ�������" << endl;
	}

	/*2.�󶨽��տͻ������ӵĶ˿� bind*/
	// sockaddr_in��sockaddr����ȷ�����Ը��Ӻ���
	sockaddr_in my_sockaddr_in = {};
	// ����sock_in�����ͣ�Ҫ��my_sockһ��
	my_sockaddr_in.sin_family = AF_INET;
	// ����sock_in�Ķ˿ںţ�����htons�����������е���������ת��Ϊ�����е���������
	my_sockaddr_in.sin_port = htons(4567);
	// �������е�ĳ��ip��ַ��INADDR_ANY��Ϊ��������ip��ַ
	my_sockaddr_in.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(my_sock, (sockaddr*)&my_sockaddr_in, sizeof(sockaddr_in)) == SOCKET_ERROR) {
		cout << "�˿ڰ�ʧ�ܡ�����" << endl;
	}
	else {
		cout << "�˿ڰ󶨳ɹ�������" << endl;
	}

	/*3.��������˿� listen*/
	// ���ڵȴ����ӵĶ��г��ȣ�5
	if (listen(my_sock, 5) == SOCKET_ERROR) {
		cout << "�˿ڼ���ʧ�ܡ�����" << endl;
	}
	else {
		cout << "�󶨼����ɹ�������" << endl;
	}

	/*4.�ȴ����տͻ������� accept*/
	sockaddr_in client_sockaddr = {};
	int client_sockaddr_len = sizeof(client_sockaddr);
	// �����ȴ�
	SOCKET client_sock = accept(my_sock, (sockaddr*)&client_sockaddr, &client_sockaddr_len);
	// INVALID_SOCKET��Ϊ��Ч��SOCKET
	if (client_sock == INVALID_SOCKET) {
		cout << "�ͻ�������ʧ�ܡ�����" << endl;
	}
	else {
		// inet_ntoa���԰������е���������ipת��Ϊ�����е���������
		cout << "socket��" << (int)client_sock << "���ӳɹ�������" << endl;
	}

	char rec_buf[256] = {};
	while (true) {
		/*5.���ܿͻ�������*/
		int rec_buf_len = recv(client_sock, rec_buf, 256, 0);
		if (rec_buf_len <= 0) {
			cout << "�ͻ������˳�������" << endl;
			break;
		}

		cout << "�յ����" << rec_buf << endl;
		/*6.��������*/
		if (!strcmp(rec_buf, "getName")) {
			char msg_buf[256] = "����";
			/*7.��ͻ��˷���һ������ send*/
			send(client_sock, msg_buf, strlen(msg_buf) + 1, 0);
		}
		else if (!strcmp(rec_buf, "getAge")) {
			char msg_buf[256] = "20";
			/*7.��ͻ��˷���һ������ send*/
			send(client_sock, msg_buf, strlen(msg_buf) + 1, 0);
		}
		else {
			char msg_buf[256] = "����������Ƿ����󡣡���";
			/*7.��ͻ��˷���һ������ send*/
			send(client_sock, msg_buf, strlen(msg_buf) + 1, 0);
		}
	}

	/*8.�ر�socket closesocket*/
	closesocket(client_sock);
	closesocket(my_sock);

	WSACleanup();
	system("pause");
	return 0;
}