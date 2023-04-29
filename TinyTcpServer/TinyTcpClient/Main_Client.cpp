#include "Client.h"
#include <thread>

void input(Client client) {
	while (true) {
		char text[256];
		cin >> text;
		if (text[0] == '/') {
			if (0 == strcmp(text, "/exit")) {
				client.Close();
				break;
			}
			else if (0 == strcmp(text, "/login")) {
				UserInfo info = {};
				info._type = CMD_LOGIN;
				strcpy(info._username, "xiaomu");
				strcpy(info._password, "123456");
				client.doSend(&info);
			}
			else if (0 == strcmp(text, "/logout")) {
				UserInfo info = {};
				info._type = CMD_LOGOUT;
				strcpy(info._username, "xiaomu");
				client.doSend(&info);
			}
			else cout << "��֧�ֵ�����" << endl;
		}
		else {
			Message msg = {};
			strcpy(msg._message, text);
			client.doSend(&msg);
		}
	}
}

void sendMsg(Client client) {
	while (client.isRun()) {
		Message msg = {};
		while (true) client.doSend(&msg);
	}
}

int main() {
	Client client;
	cout << (client.initSocket() ? "socket�����ɹ�" : "socket����ʧ��") << endl;
	cout << (client.doConnect("127.0.0.1", 10001) ? "��������ӳɹ�" : "���������ʧ��") << endl;
	// cout << (client.doConnect("59.110.170.223", 10001) ? "��������ӳɹ�" : "���������ʧ��") << endl;

	thread th_1(input, client);
	th_1.detach();

	thread th_2(sendMsg, client);
	th_2.detach();

	while (client.isRun()) client.Run({ 1,0 });

	client.Close();
	cout << "���˳�" << endl;
	system("pause");
	return 0;
}