#include "Server.h"
#include "HighTimer.h"

HighTimer timer;
Server server("127.0.0.1", 6811);

void printInfo() {
	if (timer.getSec() > 1.0f) {
		timer.upData();
		printf("client<%d>, recv_count<%d>, recv_pkg<%d>, send_count<%d>, send_pkg<%d>\n", server.clientCount(), (int)server._recv_count, (int)server._recv_pkg, (int)server._send_count, (int)server._send_pkg);
		server._recv_count = server._recv_pkg = server._send_count = server._send_pkg = 0;
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