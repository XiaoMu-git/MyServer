#include "Server.h"

int main() {
	Server server("127.0.0.1", 6811);
	server.Start(2);

	system("pause");
	return 0;
}