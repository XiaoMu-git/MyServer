#include "Server.h"

SYSTEMTIME sysTime = { 0 };

bool processMessages(SOCKET client_socket) {

}

int main() {
	GetSystemTime(&sysTime);

	/*建立服务器 socket*/
	


	/*监听服务器端口*/

	while (true) {
		GetSystemTime(&sysTime);
		fd_set fd_read;
		FD_ZERO(&fd_read);
		FD_SET(server_socket, &fd_read);

		// 将新连接的 socket 加入 fd_read
		for (auto client : clients_info) FD_SET(client.socket, &fd_read);

		/*开启 select 服务*/
		timeval time_val = { 1, 0 };
		if (select(server_socket + 1, &fd_read, NULL, NULL, &time_val) < 0) {
			printf("[%02d:%02d:%02d]", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond);
			printf("select服务结束\n");
			break;
		}

		// 判断服务器 socket 存活
		if (FD_ISSET(server_socket, &fd_read)) {
			FD_CLR(server_socket, &fd_read);
			/*等待客户端连接*/
			sockaddr_in client_sockaddr_in;
			int client_sockaddr_in_len = sizeof(sockaddr_in);
			SOCKET client_socket = accept(server_socket, (sockaddr*)&client_sockaddr_in, &client_sockaddr_in_len);
			printf("[%02d:%02d:%02d]", sysTime.wHour + 8, sysTime.wMinute, sysTime.wSecond);
			if (client_socket == INVALID_SOCKET) {
				printf("错误，收到无效客户端sock\n");
			}
			else {
				printf("新客户端加入<Socket=%d><IP=%s>\n", client_socket, inet_ntoa(client_sockaddr_in.sin_addr));
				UserInfo new_client;
				new_client.socket = client_socket;
				clients_info.push_back(new_client);
			}
		}

		// 移除容器中断连的客户端
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

	/*关闭所有 socket*/
	for (auto client : clients_info) closesocket(client.socket);
	closesocket(server_socket);

	/*关闭 Windows socket 2.2环境*/
	system("pause");
	return 0;
}
