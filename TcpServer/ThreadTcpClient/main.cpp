#include "DataType.h"
#include "Client.h"
#include "HighTimer.h"

#define CLIENT_COUNT 10

Client* clients[CLIENT_COUNT];
HighTimer timer;

void printInfo() {
	float differ = timer.getSec();
	if (differ >= 1.0f) {
		int recv_count = 0, recv_pkg = 0;
		int send_count = 0, send_pkg = 0;
		for (int i = 0; i < CLIENT_COUNT; i++) {
			recv_count += clients[i]->_recv_count; clients[i]->_recv_count = 0;
			recv_pkg += clients[i]->_recv_pkg; clients[i]->_recv_pkg = 0;
			send_count += clients[i]->_send_count; clients[i]->_send_count = 0;
			send_pkg += clients[i]->_send_pkg; clients[i]->_send_pkg = 0;
		}
		printf("¿Í»§¶Ë: thread<%d>, recv_count<%d>, recv_pkg<%d>, send_count<%d>, send_pkg<%d>\n", CLIENT_COUNT, recv_count, recv_pkg, send_count, send_pkg);
		timer.upData();
	}
}

int main() {
	for (int i = 0; i < CLIENT_COUNT; i++) {
		clients[i] = new Client("127.0.0.1", 6811);
		clients[i]->doThread();
	}

	while (true) {
		printInfo();
		std::chrono::microseconds t(1);
		std::this_thread::sleep_for(t);
	}
	return 0;
}
