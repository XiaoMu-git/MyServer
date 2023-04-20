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

	/*用Socket API建立简易TCP客户端*/
	/*1.建立一个socket*/
	SOCKET my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == my_sock) {
		cout << "Socket创建失败。。。" << endl;
	}
	else {
		cout << "Socket创建成功。。。" << endl;
	}

	/*2.连接服务器 connect*/
	sockaddr_in my_sockaddr = {};
	my_sockaddr.sin_family = AF_INET;
	my_sockaddr.sin_port = htons(4567);
	my_sockaddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (connect(my_sock, (sockaddr*)&my_sockaddr, sizeof(sockaddr_in)) == SOCKET_ERROR) {
		cout << "连接服务器失败。。。" << endl;
	}
	else {
		cout << "连接服务器成功。。。" << endl;
	}

	/*3.接收服务器信息 recv*/
	char rec_buf[256];
	recv(my_sock, rec_buf, 256, 0);
	cout << rec_buf << endl;

	/*4.关闭socket closesocket*/
	closesocket(my_sock);

	WSACleanup();
	system("pause");
	return 0;
}