#include "DataPackage.h"
#include "Client.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <thread>
using namespace std;	

void doLogin(SOCKET client_socket) {
	while (true) {
		
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
			// 命令区块
		}
		else send(client_socket, (char*)&recv_message, recv_message.length, 0);
	}
}

bool processMessages(SOCKET client_socket) {

	}
}

int main() {

	/*连接服务器*/
	


	/*开启多线程用来输入 thread*/
	std::thread putIn(cmdThread, client_socket);

	while (true) {

		}
	}


	
	system("pause");
	return 0;
}