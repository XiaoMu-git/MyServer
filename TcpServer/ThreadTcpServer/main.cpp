#include "Server.h"

int main() {
	Server server;
	puts(server.doSocket() ? "socket初始化成功。" : "socket初始化失败。");
	puts(server.doBind("127.0.0.1", 6811) ? "端口绑定成功。" : "端口绑定失败。");
	puts(server.doListen() ? "端口监听成功。" : "端口监听失败。");
	server.Start(4);

	system("pause");
	return 0;
}