#include "Server.h"
#include "HighTimer.h"

HighTimer timer; 
Server server("127.0.0.1", 6811, 1);

void printInfo() {
	if (timer.getSec() > 1.0f) {
		timer.upData();
		auto recv = server.recvCount();
		auto send = server.sendCount();
		printf("client<%d>, recv_count<%d>, recv_pkg<%d>, send_count<%d>, send_pkg<%d>\n", server.clientCount(), recv.first, recv.second, send.first, send.second);
	}
}

int main() {

	server.doThread();

	while (true) {
		printInfo();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	return 0;
}