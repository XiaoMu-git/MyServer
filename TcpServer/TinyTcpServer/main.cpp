#include "Server.h"

int main() {
	Server server("127.0.0.1", 6811);
	server.Start();

	system("pause");
	return 0;
}