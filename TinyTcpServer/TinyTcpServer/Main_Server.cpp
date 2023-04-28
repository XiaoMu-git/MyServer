#include "Server.h"

int main() {
	Server server;
	cout << (server.initSocket() ? "socket创建成功" : "socket创建失败") << endl;
	cout << (server.doBind(nullptr, 10001) ? "端口绑定成功" : "端口绑定失败") << endl;
	cout << (server.doListen(5) ? "端口监听成功" : "端口监听失败") << endl;

	while (server.isRun()) server.Run({ 1,0 });
	server.Close();
	system("pause");
	return 0;
}