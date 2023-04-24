#include "Server.h"

SYSTEMTIME sysTime = { 0 };

bool processMessages(SOCKET client_socket) {

}

int main() {
	GetSystemTime(&sysTime);

	/*���������� socket*/
	


	/*�����������˿�*/

	while (true) {
		GetSystemTime(&sysTime);
		fd_set fd_read;
		FD_ZERO(&fd_read);
		FD_SET(server_socket, &fd_read);

		// �������ӵ� socket ���� fd_read
		for (auto client : clients_info) FD_SET(client.socket, &fd_read);

		/*���� select ����*/
		timeval time_val = { 1, 0 };
		if (select(server_socket + 1, &fd_read, NULL, NULL, &time_val) < 0) {
			printf("[%02d:%02d:%02d]", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond);
			printf("select�������\n");
			break;
		}

		// �жϷ����� socket ���
		if (FD_ISSET(server_socket, &fd_read)) {
			FD_CLR(server_socket, &fd_read);
			/*�ȴ��ͻ�������*/
			sockaddr_in client_sockaddr_in;
			int client_sockaddr_in_len = sizeof(sockaddr_in);
			SOCKET client_socket = accept(server_socket, (sockaddr*)&client_sockaddr_in, &client_sockaddr_in_len);
			printf("[%02d:%02d:%02d]", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond);
			if (client_socket == INVALID_SOCKET) {
				printf("�����յ���Ч�ͻ���sock\n");
			}
			else {
				printf("�¿ͻ��˼���<Socket=%d><IP=%s>\n", client_socket, inet_ntoa(client_sockaddr_in.sin_addr));
				UserInfo new_client;
				new_client.socket = client_socket;
				clients_info.push_back(new_client);
			}
		}

		// �Ƴ������ж����Ŀͻ���
		for (int i = 0; i < fd_read.fd_count; i++) {
			if (!processMessages(fd_read.fd_array[i])) {
				for (auto iter = clients_info.begin(); iter != clients_info.end(); iter++) {
					if ((*iter).socket == fd_read.fd_array[i]) {
						clients_info.erase(iter);
						break;
					}
				}
			}
		}
	}

	/*�ر����� socket*/
	for (auto client : clients_info) closesocket(client.socket);
	closesocket(server_socket);

	/*�ر� Windows socket 2.2����*/
	system("pause");
	return 0;
}
