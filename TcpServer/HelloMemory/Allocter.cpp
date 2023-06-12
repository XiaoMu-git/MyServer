#include "Allocter.h"

void* operator new (size_t size) {
	return MemoryManager::instance().mmalloc(size);
}

void operator delete (void* ptr) {
	MemoryManager::instance().ffree(ptr);
}

void* operator new[] (size_t size) {
	return MemoryManager::instance().mmalloc(size);
}

void operator delete[](void* ptr) {
	MemoryManager::instance().ffree(ptr);
}
