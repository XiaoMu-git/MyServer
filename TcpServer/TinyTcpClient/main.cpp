#include "Client.h"

const int N = 1024;

int main() {
	Client* client[N];

	for (int i = 0; i < N; i++) {
		client[i] = new Client();
		if (client[i]->initSocket()) {
			cout << "[" << i + 1 << "]<socket=" << client[i]->Socket() << ">socket创建成功, ";
			// 59.110.170.223
			// 127.0.0.1
			if (client[i]->doConnect("127.0.0.1", 6811)) {
				cout << "[" << i + 1 << "]<socket=" << client[i]->Socket() << ">服务端连接成功" << endl;
				client[i]->isConnect(true);
			}
			else cout << "[" << i + 1 << "]<socket=" << client[i]->Socket() << ">服务端连接失败" << endl;
		}
		else cout << "[" << i + 1 << "]socket创建失败" << endl;
	}

	Header header = {};
	header._type = CMD_LOGIN;
	while (true) {
		for (int i = 0; i < N; i++) {
			if (client[i]->isConnect()) {
				client[i]->doSend(&header);
			}
		}
	}

	cout << "已退出" << endl;
	system("pause");
	return 0;
}