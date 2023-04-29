#include "Server.h"
#include <thread>

void input(Server server) {
	while (true) {
		char text[256];
		cin >> text;
		if (text[0] == '/') {
			if (0 == strcmp(text, "/exit")) {
				server.Close();
				break;
			}
			else cout << "不支持的命令" << endl;
		}
		else {
			Message msg = {};
			strcpy(msg._message, text);
			server.doSendAll(&msg);
		}
	}
}

int main() {
	Server server;
	cout << (server.initSocket() ? "socket创建成功" : "socket创建失败") << endl;
	cout << (server.doBind(nullptr, 10001) ? "端口绑定成功" : "端口绑定失败") << endl;
	cout << (server.doListen(5) ? "端口监听成功" : "端口监听失败") << endl;

	thread th_1(input, server);
	th_1.detach();

	while (server.isRun()) server.Run({ 1,0 });
	server.Close();
	system("pause");
	return 0;
}