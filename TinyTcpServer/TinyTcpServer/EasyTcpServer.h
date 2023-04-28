#ifndef _EasyTcpServer_h_
#define _EasyTcpServer_h_

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<windows.h>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>

#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#endif

#include "MessageHeader.h"
#include<stdio.h>
#include<vector>

class EasyTcpServer {
	SOCKET _sock;
	std::vector<SOCKET> _clients;


public:
	EasyTcpServer() {
		_sock = INVALID_SOCKET;
	}

	~EasyTcpServer() {
		Close();
	}

	SOCKET InitSocket() {
#ifdef _WIN32
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat);
#endif
		if (INVALID_SOCKET != _sock) {
			printf("<socket=%d>关闭旧连接...\n", _sock);
			Close();
		}
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == _sock) {
			printf("错误，建立<socket=%d>失败\n", _sock);
		}
		else {
			printf("建立<socket=%d>成功...\n", _sock);
		}
		return _sock;
	}

	int Bind(const char* ip, unsigned short port) {
		if (INVALID_SOCKET == _sock) InitSocket();
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);
#ifdef _WIN32
		if (ip) _sin.sin_addr.S_un.S_addr = inet_addr(ip);
		else _sin.sin_addr.S_un.S_addr = INADDR_ANY;
#else
		if (ip) _sin.sin_addr.s_addr = inet_addr(ip);
		else _sin.sin_addr.s_addr = INADDR_ANY;
#endif
		int ret = bind(_sock, (sockaddr*)&_sin, sizeof(_sin));
		if (SOCKET_ERROR == ret)
		{
			printf("错误,绑定网络端口<%d>失败...\n", port);
		}
		else {
			printf("绑定网络端口<%d>成功...\n", port);
		}
		return ret;
	}

	int Listen(int num) {
		int ret = listen(_sock, num);
		if (SOCKET_ERROR == ret)
		{
			printf("<socket=%d>错误,监听网络端口失败...\n", _sock);
		}
		else {
			printf("<socket=%d>监听网络端口成功...\n", _sock);
		}
		return ret;
	}

	void Close() {
		if (INVALID_SOCKET == _sock) {
#ifdef _WIN32
			for (int n = (int)_clients.size() - 1; n >= 0; n--) {
				closesocket(_clients[n]);
			}
			closesocket(_sock);
			WSACleanup();
#else
			for (int n = (int)g_clients.size() - 1; n >= 0; n--) {
				close(_clients[n]);
			}
			close(_sock);
#endif
		}
	}

	int Accept() {
		sockaddr_in clientAddr = {};
		int nAddrLen = sizeof(sockaddr_in);
		SOCKET _cSock = INVALID_SOCKET;
#ifdef _WIN32
		_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
#else
		_cSock = accept(_sock, (sockaddr*)&clientAddr, (socklen_t*)&nAddrLen);
#endif
		if (INVALID_SOCKET == _cSock)
		{
			printf("错误,接受到无效客户端<socket=%d>...\n", _cSock);
		}
		else
		{
			NewUserJoin userJoin;
			SendDataAll(&userJoin);
			_clients.push_back(_cSock);
			printf("新客户端加入：<socket=%d>,<IP=%s> \n", (int)_cSock, inet_ntoa(clientAddr.sin_addr));
		}
		return _cSock;
	}

	bool isRun() {
		return INVALID_SOCKET != _sock;
	}

	bool OnRun() {
		if (isRun()) {
			fd_set fdRead;
			fd_set fdWrite;
			fd_set fdExp;
			FD_ZERO(&fdRead);
			FD_ZERO(&fdWrite);
			FD_ZERO(&fdExp);
			FD_SET(_sock, &fdRead);
			FD_SET(_sock, &fdWrite);
			FD_SET(_sock, &fdExp);
			SOCKET maxSock = _sock;
			for (int n = (int)_clients.size() - 1; n >= 0; n--)
			{
				FD_SET(_clients[n], &fdRead);
				if (maxSock < _clients[n])
				{
					maxSock = _clients[n];
				}
			}
			timeval t = { 1,0 };
			int ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExp, &t);
			if (ret < 0) {
				printf("select任务结束。\n");
				return false;
			}
			if (FD_ISSET(_sock, &fdRead)) {
				FD_CLR(_sock, &fdRead);
				Accept();
			}
			for (int n = (int)_clients.size() - 1; n >= 0; n--) {
				if (FD_ISSET(_clients[n], &fdRead)) {
					if (-1 == RecvData(_clients[n])) {
						auto iter = _clients.begin() + n;
						if (iter != _clients.end())
						{
							_clients.erase(iter);
						}
					}
				}
			}
			return true;
		}
		return false;

	}

	int RecvData(SOCKET _cSock)
	{
		char szRecv[4096] = {};
		int nLen = (int)recv(_cSock, szRecv, sizeof(DataHeader), 0);
		DataHeader* header = (DataHeader*)szRecv;
		if (nLen <= 0) {
			printf("客户端<Socket=%d>已退出，任务结束。\n", _cSock);
			return -1;
		}
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		OnNetMsg(_cSock, header);
		return 0;
	}

	void OnNetMsg(SOCKET _cSock, DataHeader* header) {
		switch (header->cmd)
		{
		case CMD_LOGIN: {
			Login* login = (Login*)header;
			printf("收到客户端<Socket=%d>请求：CMD_LOGIN,数据长度：%d,userName=%s PassWord=%s\n", _cSock, login->dataLength, login->userName, login->PassWord);
			LoginResult ret;
			send(_cSock, (char*)&ret, sizeof(LoginResult), 0);
		}
					  break;
		case CMD_LOGOUT: {
			Logout* logout = (Logout*)header;
			printf("收到客户端<Socket=%d>请求：CMD_LOGOUT,数据长度：%d,userName=%s \n", _cSock, logout->dataLength, logout->userName);
			LogoutResult ret;
			send(_cSock, (char*)&ret, sizeof(ret), 0);
		}
					   break;
		default: {
			DataHeader header = { 0,CMD_ERROR };
			send(_cSock, (char*)&header, sizeof(header), 0);
		}
			   break;
		}
	}

	int SendData(SOCKET _cSock, DataHeader* header) {
		if (isRun(), header) {
			return send(_cSock, (const char*)&header, header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}

	void SendDataAll(DataHeader* header) {
		for (int n = (int)_clients.size() - 1; n >= 0; n--) {
			SendData(_clients[n], header);
		}
	}
};

#endif // !_EasyTcpServer_h_
