#include "Server.h"

int main() {
	Server server;
	cout << (server.initSocket() ? "socket�����ɹ�" : "socket����ʧ��") << endl;
	cout << (server.doBind(nullptr, 10001) ? "�˿ڰ󶨳ɹ�" : "�˿ڰ�ʧ��") << endl;
	cout << (server.doListen(5) ? "�˿ڼ����ɹ�" : "�˿ڼ���ʧ��") << endl;

	while (server.isRun()) server.Run({ 1,0 });
	server.Close();
	system("pause");
	return 0;
}