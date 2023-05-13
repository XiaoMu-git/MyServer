#include "DataType.h"
#include "Client.h"
#include <iostream>
#include <thread>

const int client_num = 4000;
const int thread_num = 4;
const char ip[] = "127.0.0.1";
// const char ip[] = "59.110.170.223";

void sendMsg(int id) {
	Client* clients[client_num];
	int connect_num = 0;
	for (int i = 0; i < client_num / thread_num; i++) {
		clients[i] = new Client();
		if (clients[i]->doConnect(ip, 6811)) {
			clients[i]->Connect(true);
			connect_num++;
		}
		else printf("thread<%d>, socket<%d>, Connection failed.\n", id, clients[i]->Socket());
	}
	printf("thread<%d>, client<%d>, Connections were completed.\n", id, connect_num);

	Header* header;
	while (true) {
		for (int i = 0; i < client_num / thread_num; i++) {
			if (clients[i]->Connect()) clients[i]->doSend(header, header->_length);
		}
	}
}

int main() {
	std::thread* th[thread_num];
	for (int i = 0; i < thread_num; i++) {
		th[i] = new std::thread(sendMsg, i);
		th[i]->detach();
	}

	while (true) Sleep(100);
	system("pause");
	return 0;
}