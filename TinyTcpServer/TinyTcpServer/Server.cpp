#include "EasyTcpServer.h"

int main() {
	EasyTcpServer server;
	server.InitSocket();
	server.Bind(nullptr, 10001);
	server.Listen(5);
	while (server.isRun()) server.OnRun();
	server.Close();
	system("pause");
	return 0;
}