#include "Client.h"

const int N = 1024;

int main() {
	Client* client[N];

	for (int i = 0; i < N; i++) {
		client[i] = new Client();
		if (client[i]->initSocket()) {
			cout << "[" << i + 1 << "]<socket=" << client[i]->Socket() << ">socket�����ɹ�, ";
			// 59.110.170.223
			// 127.0.0.1
			if (client[i]->doConnect("127.0.0.1", 6811)) {
				cout << "[" << i + 1 << "]<socket=" << client[i]->Socket() << ">��������ӳɹ�" << endl;
				client[i]->isConnect(true);
			}
			else cout << "[" << i + 1 << "]<socket=" << client[i]->Socket() << ">���������ʧ��" << endl;
		}
		else cout << "[" << i + 1 << "]socket����ʧ��" << endl;
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

	cout << "���˳�" << endl;
	system("pause");
	return 0;
}