#include "Allocter.h"

void* operator new (size_t size) {
	printf("new memory<%d>\n", size);
	return malloc(size);
}

void operator delete (void* pointer) {
	printf("delete memory\n");
	free(pointer);
}

void* operator new[] (size_t size) {
	printf("new[] memory<%d>\n", size);
	return malloc(size);
}

void operator delete[](void* pointer) {
	printf("delete[] memory\n");
	free(pointer);
}
