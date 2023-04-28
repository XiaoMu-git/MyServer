#include "EasyTcpClient.h"
#include <thread>

void cmdThread(EasyTcpClient* client)
{
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit")) {
			client->Close();
			printf("退出cmdThread线程\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login")) {
			Login login;
			strcpy(login.userName, "lyd");
			strcpy(login.PassWord, "lydmm");
			client->SendData(&login);
		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			Logout logout;
			strcpy(logout.userName, "lyd");
			client->SendData(&logout);
		}
		else {
			printf("不支持的命令。\n");
		}
	}
}

int main() {
	EasyTcpClient client;
	client.Connect("127.0.0.1", 10001);

	std::thread th_1(cmdThread, &client);
	th_1.detach();

	while (client.isRun()) {
		client.OnRun();
	}
	client.Close();
	printf("已退出...\n");
	system("pause");
	return 0;
}