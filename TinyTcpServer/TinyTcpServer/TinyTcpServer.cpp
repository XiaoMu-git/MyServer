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

	/*用Socket API建立简易TCP服务端，并实现消息处理回传*/
	/*1.建立一个socket*/
	// 使用的协议簇：AF_INET，socket类型：SOCK_STREAM，协议的种类：IPPROTO_TCP
	SOCKET my_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == my_sock) {
		cout << "socket创建失败。。。" << endl;
	}
	else {
		cout << "socket创建成功。。。" << endl;
	}

	/*2.绑定接收客户端连接的端口 bind*/
	// sockaddr_in比sockaddr更明确，所以更加好用
	sockaddr_in my_sockaddr_in = {};
	// 设置sock_in的类型，要与my_sock一致
	my_sockaddr_in.sin_family = AF_INET;
	// 设置sock_in的端口号，利用htons函数将主机中的数据类型转换为网络中的数据类型
	my_sockaddr_in.sin_port = htons(4567);
	// 绑定主机中的某个ip地址，INADDR_ANY意为主机所有ip地址
	my_sockaddr_in.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(my_sock, (sockaddr*)&my_sockaddr_in, sizeof(sockaddr_in)) == SOCKET_ERROR) {
		cout << "端口绑定失败。。。" << endl;
	}
	else {
		cout << "端口绑定成功。。。" << endl;
	}

	/*3.监听网络端口 listen*/
	// 正在等待连接的队列长度：5
	if (listen(my_sock, 5) == SOCKET_ERROR) {
		cout << "端口监听失败。。。" << endl;
	}
	else {
		cout << "绑定监听成功。。。" << endl;
	}

	/*4.等待接收客户端连接 accept*/
	sockaddr_in client_sockaddr = {};
	int client_sockaddr_len = sizeof(client_sockaddr);
	// 阻塞等待
	SOCKET client_sock = accept(my_sock, (sockaddr*)&client_sockaddr, &client_sockaddr_len);
	// INVALID_SOCKET意为无效的SOCKET
	if (client_sock == INVALID_SOCKET) {
		cout << "客户端连接失败。。。" << endl;
	}
	else {
		// inet_ntoa可以把网络中的数据类型ip转换为主机中的数据类型
		cout << "socket：" << (int)client_sock << "连接成功。。。" << endl;
	}

	char rec_buf[256] = {};
	while (true) {
		/*5.接受客户端数据*/
		int rec_buf_len = recv(client_sock, rec_buf, 256, 0);
		if (rec_buf_len <= 0) {
			cout << "客户端已退出。。。" << endl;
			break;
		}

		cout << "收到命令：" << rec_buf << endl;
		/*6.处理请求*/
		if (!strcmp(rec_buf, "getName")) {
			char msg_buf[256] = "张三";
			/*7.向客户端发送一条数据 send*/
			send(client_sock, msg_buf, strlen(msg_buf) + 1, 0);
		}
		else if (!strcmp(rec_buf, "getAge")) {
			char msg_buf[256] = "20";
			/*7.向客户端发送一条数据 send*/
			send(client_sock, msg_buf, strlen(msg_buf) + 1, 0);
		}
		else {
			char msg_buf[256] = "经检查输入是否有误。。。";
			/*7.向客户端发送一条数据 send*/
			send(client_sock, msg_buf, strlen(msg_buf) + 1, 0);
		}
	}

	/*8.关闭socket closesocket*/
	closesocket(client_sock);
	closesocket(my_sock);

	WSACleanup();
	system("pause");
	return 0;
}