#include "DataType.h"
#include "Client.h"
#include "HighTimer.h"
#include <thread>
#include <atomic>

const char ip[] = "127.0.0.1";
// const char ip[] = "59.110.170.223";
const int client_num = 1000;
const int thread_num = 2;
std::atomic_int alive_client_num = 0;
std::atomic_int send_num = 0;
HighTimer timer;

// 处理客户端消息
void timeMsg() {
	float time_interval = timer.getSec();
	if (time_interval >= 1.0f) {
		timer.upData();
		printf("客户端：thread<%d>,client<%d>,send<%d>\n", thread_num, (int)alive_client_num, int(send_num / time_interval));
		send_num = 0;
	}
}

// 连接服务器和发送数据
void sendMsg(int id) {
	Client* clients[client_num];
	for (int i = 0; i < client_num; i++) {
		clients[i] = new Client();
		if (clients[i]->doConnect(ip, 6811)) {
			clients[i]->Connect(true);
			alive_client_num++;
		}
		else printf("thread<%d>, socket<%d>, Connection failed.\n", id, clients[i]->Socket());
	}

	Message* message = new Message();
	while (true) {
		for (int i = 0; i < client_num; i++) {
			if (clients[i]->Connect()) {
				send_num++;
				clients[i]->doSend(message);
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

	while (true) {
		timeMsg();
		Sleep(100);
	}
	system("pause");
	return 0;
}