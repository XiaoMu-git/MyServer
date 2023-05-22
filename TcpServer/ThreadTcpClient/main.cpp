#include "Client.h"

#define CLIENT_NUM 10
#define THRED_NUM 2
HighTimer timer;
int recv_num = 0, recv_pkg = 0;
int send_num = 0, send_pkg = 0;

void printInfo() {
	float differ = timer.getSec();
	if (differ >= 1.0f) {
		printf("¿Í»§¶Ë£ºthread<%d>,recv<%d>,recv_pkg<%d>,send<%d>,send_pkg<%d>\n", THRED_NUM, (int)(recv_num / differ), (int)(recv_pkg / differ), (int)(send_num / differ), (int)(send_pkg / differ));
		recv_num = recv_pkg = 0;
		send_num = send_pkg = 0;
		timer.upData();
	}
}

void sendFun() {
	Message* msg = new Message();
	std::vector<Client*> clients;
	std::vector<Client*> delete_clients;
	for (int i = 0; i < CLIENT_NUM; i++) {
		Client* client = new Client("127.0.0.1", 6811);
		client->Start();
		clients.push_back(client);
	}

	while (true) {
		for (auto client : clients) {
			recv_num += client->_recv_num; client->_recv_num = 0;
			recv_pkg += client->_recv_pkg; client->_recv_pkg = 0;
			send_num += client->_send_num; client->_send_num = 0;
			send_pkg += client->_send_pkg; client->_send_pkg = 0;
			if (!client->doRun(msg)) {
				delete_clients.push_back(client);
			}
		}

		for (auto client : delete_clients) {
			for (auto iter = clients.begin(); iter != clients.end(); iter++) {
				if ((*iter)->_socket == client->_socket) {
					delete client;
					clients.erase(iter);
					break;
				}
			}
		}
		delete_clients.clear();
	}
}

int main() {
	for (int i = 0; i < THRED_NUM; i++) {
		std::thread th(&sendFun);
		th.detach();
	}

	while (true) {
		printInfo();
		std::chrono::microseconds t(1);
		std::this_thread::sleep_for(t);
	}

	system("pause");
	return 0;
}