#include "DataType.h"
#include "Client.h"
#include "HighTimer.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

const int client_num = 200;
const int thread_num = 4;
const char ip[] = "127.0.0.1";
// const char ip[] = "59.110.170.223";

bool allRun = false;
std::mutex my_mutex;

void sendMsg(int id) {
	Client* clients[client_num];
	for (int i = 0; i < client_num; i++) {
		clients[i] = new Client();
		if (clients[i]->doInit()) {
			if (clients[i]->doConnect(ip, 6811)) {
				clients[i]->Connect(true);
			}
			else printf("thread<%d>, socket<%d>, Connection failed.\n", id, clients[i]->Socket());
		}
		else printf("thread<%d>, Create failed.\n", id);
	}

	Header header = {};
	while (true) {
		for (int i = 0; i < client_num; i++) {
			if (clients[i]->Connect()) {
				clients[i]->doSend(&header);
			}
		}
	}
}

int main() {
	std::thread* th[thread_num];
	for (int i = 0; i < thread_num; i++) {
		th[i] = new std::thread(sendMsg, i);
		th[i]->detach();
	}

	while (true);
	system("pause");
	return 0;
}