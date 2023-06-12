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
	MemoryPoolTempLate<32, 1000> _mem32;
	MemoryPoolTempLate<64, 1000> _mem64;
	MemoryPoolTempLate<128, 1000> _mem128;
	MemoryPoolTempLate<256, 1000> _mem256;
	std::mutex _mutex;

	MemoryManager();

	~MemoryManager();

public:
	static MemoryManager& instance();

	void* mmalloc(size_t size);

	void ffree(void* ptr);
};

#endif // !_MemoryPool_h_
