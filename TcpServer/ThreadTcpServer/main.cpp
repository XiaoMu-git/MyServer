#include "Server.h"

int main() {
	Server server;
	puts(server.doSocket() ? "socket��ʼ���ɹ���" : "socket��ʼ��ʧ�ܡ�");
	puts(server.doBind("127.0.0.1", 6811) ? "�˿ڰ󶨳ɹ���" : "�˿ڰ�ʧ�ܡ�");
	puts(server.doListen() ? "�˿ڼ����ɹ���" : "�˿ڼ���ʧ�ܡ�");
	server.Start(4);

	system("pause");
	return 0;
}