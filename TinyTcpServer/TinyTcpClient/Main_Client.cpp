#include "Client.h"
#include <thread>

void input(Client* client) {
	while (true) {
		char text[256];
		cin >> text;
		if (text[0] == '/') {
			if (0 == strcmp(text, "/exit")) {
				client->Close();
				break;
			}
			else if (0 == strcmp(text, "/login")) {
				UserInfo info = {};
				info._type = CMD_LOGIN;
				info._socket = client->getSocket();
				strcpy(info._username, "xiaomu");
				strcpy(info._password, "123456");
				client->doSend(&info);
			}
			else if (0 == strcmp(text, "/logout")) {
				UserInfo info = {};
				info._type = CMD_LOGOUT;
				info._socket = client->getSocket();
				strcpy(info._username, "xiaomu");
				client->doSend(&info);
			}
		}
		else {
			Message msg = {};
			msg._socket = client->getSocket();
			strcpy(msg._message, text);
			client->doSend(&msg);
		}
	}
}

int main() {
	Client client;

	cout << (client.initSocket() ? "socket创建成功" : "socket创建失败") << endl;
	cout << (client.doConnect("127.0.0.1", 10001) ? "服务端连接成功" : "服务端连接失败") << endl;

	thread th_1(input, &client);
	th_1.detach();

	while (client.isRun()) client.Run({ 1,0 });

	client.Close();
	cout << "已退出" << endl;
	system("pause");
	return 0;
}