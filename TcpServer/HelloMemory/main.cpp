#include "Allocter.h"

int main() {
	char* a = new char;
	delete a;

	char* b = new char[64];
	delete[] b;

	char* c = (char*)malloc(64);
	free(c);

	system("pause");
	return 0;
}