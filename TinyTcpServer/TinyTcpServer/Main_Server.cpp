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
			else cout << "��֧�ֵ�����" << endl;
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
	cout << (server.initSocket() ? "socket�����ɹ�" : "socket����ʧ��") << endl;
	cout << (server.doBind(nullptr, 10001) ? "�˿ڰ󶨳ɹ�" : "�˿ڰ�ʧ��") << endl;
	cout << (server.doListen(5) ? "�˿ڼ����ɹ�" : "�˿ڼ���ʧ��") << endl;

	thread th_1(input, server);
	th_1.detach();

	while (server.isRun()) server.Run({ 1,0 });
	server.Close();
	system("pause");
	return 0;
}