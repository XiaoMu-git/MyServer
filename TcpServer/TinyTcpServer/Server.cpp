#include "Server.h"

Server::Server(const char* ip, unsigned short port) {
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	_ip = ip;
	_port = port;
	_timer.upData();
	_socket = INVALID_SOCKET;
	_recv_num = _recv_pkg = 0;
	_send_num = _send_pkg = 0;
}

// ��ʼ�� socket
bool Server::initSocket() {
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	return _socket != INVALID_SOCKET;
}

// �󶨶˿�
bool Server::doBind() {
	sockaddr_in addr_in = {};
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(_port);
#ifdef _WIN32
	if (_ip) addr_in.sin_addr.S_un.S_addr = inet_addr(_ip);
	else addr_in.sin_addr.S_un.S_addr = INADDR_ANY;
#else
	if (_ip) addr_in.sin_addr.s_addr = inet_addr(_ip);
	else addr_in.sin_addr.s_addr = INADDR_ANY;
#endif
	int ret = bind(_socket, (sockaddr*)&addr_in, sizeof(addr_in));
	return ret != SOCKET_ERROR;
}

// �����˿�
bool Server::doListen(int num) {
	int ret = listen(_socket, num);
	return ret != SOCKET_ERROR;
}

// �ȴ��ͻ�������
void Server::waitAccept() {
	sockaddr_in client_addr = {};
	int addr_len = sizeof(sockaddr_in);
	SOCKET client_socket;
#ifdef _WIN32
	client_socket = accept(_socket, (sockaddr*)&client_addr, &addr_len);
#else
	client_socket = accept(_socket, (sockaddr*)&client_addr, (socklen_t*)&addr_len);
#endif
	if (client_socket != INVALID_SOCKET) {
		std::lock_guard<std::mutex> lock(_mutex);
		_clients.insert({ client_socket , new Client(client_socket) });
	}
}

// ������Ϣ
int Server::doRecv(Client* client) {
	int recv_len = (int)recv(client->_socket, client->_recv_buff + client->_recv_pos, RECV_BUFF_SIZE - client->_recv_pos, 0);
	if (recv_len <= 0) return recv_len;
	int left_pos = 0;
	client->_recv_pos += recv_len;
	_recv_num++;
	while (client->_recv_pos >= sizeof(Header)) {
		Header* header = (Header*)client->_recv_buff;
		if (client->_recv_pos >= header->_size) {
			memcpy(client->_recv_buff, client->_recv_buff + header->_size, RECV_BUFF_SIZE - header->_size);
			doDispose(client, header);
			client->_recv_pos -= header->_size;
			left_pos += header->_size;
			_recv_pkg++;
		}
		else break;
	}
	memcpy(client->_recv_buff, client->_recv_buff + left_pos, client->_recv_pos);
	return true;
}

// ������Ϣ
void Server::doSend(Client* client, Header* header) {
	int remainder_len = header->_size;
	_send_pkg++;
	while (true) {
		if (client->_send_pos + remainder_len >= SEND_BUFF_SIZE) {
			int empty_len = SEND_BUFF_SIZE - client->_send_pos;
			memcpy(client->_send_buff + client->_send_pos, header + (header->_size - remainder_len), empty_len);
			send(_socket, (const char*)client->_send_buff, SEND_BUFF_SIZE, 0);
			remainder_len -= empty_len;
			client->_send_pos = 0;
			_send_num++;
		}
		else {
			memcpy(client->_send_buff + client->_send_pos, header, remainder_len);
			client->_send_pos += remainder_len;
			break;
		}
	}
}

// ������Ϣ
void Server::doDispose(Client* client, Header* header) {

}

// ���к���
void Server::doRun() {
	timeval time_val = { 0, 10 };
	while (isRun()) {
		printInfo();

		fd_set fd_reads;
		FD_ZERO(&fd_reads);
		FD_SET(_socket, &fd_reads);
		SOCKET max_socket = _socket;
		for (auto client : _clients) {
			FD_SET(client.second->_socket, &fd_reads);
			if (max_socket < client.second->_socket) max_socket = client.second->_socket;
		}

		int ret = select(max_socket + 1, &fd_reads, NULL, NULL, &time_val);
		if (ret < 0) return;

		if (FD_ISSET(_socket, &fd_reads)) {
			FD_CLR(_socket, &fd_reads);
			waitAccept();
		}

		for (int i = 0; i < fd_reads.fd_count; i++) {
			auto iter = _clients.find(fd_reads.fd_array[i]);
			if (iter != _clients.end()) {
				if (doRecv(iter->second) == -1) _clients.erase(iter);
			}
		}
	}
}

// ����������
void Server::Start() {
	if (initSocket()) printf("socket�����ɹ�...\n");
	else {
		printf("socket����ʧ��...\n");
		return;
	}

	if (doBind()) printf("�˿ڰ󶨳ɹ�...\n");
	else {
		printf("�˿ڰ�ʧ��...\n");
		return;
	}

	if (doListen(5)) printf("�����˿ڳɹ�...\n");
	else {
		printf("�����˿�ʧ��...\n");
		return;
	}

	doRun();
}

// �رշ�����
void Server::doClose() {
	if (isRun()) {
#ifdef _WIN32
		closesocket(_socket);
#else
		close(_socket);
#endif
		_socket = INVALID_SOCKET;
	}
}

// ����״̬
bool Server::isRun() {
	return _socket != INVALID_SOCKET;
}

// ��������Ϣ
void Server::printInfo() {
	float differ = _timer.getSec();
	if (differ >= 1.0f) {
		printf("��������client<%d>,recv<%d>,recv_pkg<%d>,send<%d>,send_pkg<%d>\n", _clients.size(), (int)(_recv_num / differ), (int)(_recv_pkg / differ), (int)(_send_num / differ), (int)(_send_pkg / differ));
		_recv_num = _recv_pkg = 0;
		_send_num = _send_pkg = 0;
		_timer.upData();
	}
}

Server::~Server() {
#ifdef _WIN32
	WSACleanup();
#endif
	doClose();
}