#ifndef _MemoryPool_h_
#define _MemoryPool_h_

#include <stdlib.h>
#include <mutex>

class MemoryPool;

class MemoryBlock {
public:
	unsigned int _id, _ref;
	bool _is_pool;
	MemoryPool* _pool_ptr;
	MemoryBlock* _next;
};

class MemoryPool {
protected:
	char* _space;
	size_t _block_count, _block_size;
	MemoryBlock* _header;
	std::mutex _mutex;

	MemoryPool();

	~MemoryPool();

public:

	void initMemory();

	void* mmalloc(size_t size);

	void ffree(void* ptr);
};

template<size_t block_size, size_t block_count>
class MemoryPoolTempLate : public MemoryPool {
public:
	MemoryPoolTempLate();
};

class MemoryManager {
private:
	MemoryPoolTempLate<32, 100000> _mem32;
	MemoryPoolTempLate<64, 100000> _mem64;
	MemoryPoolTempLate<128, 100000> _mem128;
	MemoryPoolTempLate<256, 100000> _mem256;
	MemoryPoolTempLate<512, 100000> _mem512;
	MemoryPoolTempLate<1024, 100000> _mem1024;
	MemoryPoolTempLate<2048, 100000> _mem2048;
	MemoryPoolTempLate<4096, 100000> _mem4096;
	MemoryPoolTempLate<8192, 100000> _mem8192;
	MemoryPoolTempLate<16384, 100000> _mem16384;
	std::mutex _mutex;

	MemoryManager();

	~MemoryManager();

public:
	static MemoryManager& instance();

	void* mmalloc(size_t size);

	void ffree(void* ptr);
};

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

#endif // !_MemoryPool_h_
