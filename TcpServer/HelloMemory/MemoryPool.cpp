#include "MemoryPool.h"

MemoryPool::MemoryPool() {
	_space = nullptr;
	_header = nullptr;
	_block_count = _block_size = 0;
}

MemoryPool::~MemoryPool() {
	if (_space != nullptr) free(_space);
}

void MemoryPool::initMemory() {
	if (_space != nullptr) return;
	size_t real_size = _block_size + sizeof(MemoryBlock);
	size_t space_size = _block_count * real_size;
	_space = (char*)malloc(space_size);
	for (size_t i = 0; i < _block_count; i++) {
		MemoryBlock* temp = (MemoryBlock*)(_space + (i * real_size));
		temp->_id = i;
		temp->_ref = 1;
		temp->_is_pool = true;
		temp->_pool_ptr = this;
		temp->_next = _header;
		_header = temp;
	}
}

void* MemoryPool::mmalloc(size_t size) {
	std::lock_guard<std::mutex> lock(_mutex);
	if (_space == nullptr) initMemory();
	MemoryBlock* block_res = nullptr;
	if (_header == nullptr) {
		block_res = (MemoryBlock*)malloc(size + sizeof(MemoryBlock));
		block_res->_id = -1;
		block_res->_ref = 1;
		block_res->_next = nullptr;
		block_res->_is_pool = false;
		block_res->_pool_ptr = nullptr;
	}
	else {
		block_res = _header;
		_header = block_res->_next;
		block_res->_ref = 1;
	}
	printf("[1]内存池申请内存 id<%d>, size<%d>, ptr<%llx>\n", block_res->_id, size, block_res);
	return (char*)block_res + sizeof(MemoryBlock);
}

void MemoryPool::ffree(void* ptr) {
	MemoryBlock* block = (MemoryBlock*)((char*)ptr - sizeof(MemoryBlock));
	if (--block->_ref == 0) {
		printf("[1]内存池释放内存 ptr<%llx>\n", block);
		std::lock_guard<std::mutex> lock(_mutex);
		block->_next = _header;
		_header = block;
	}
}

template<size_t block_size, size_t block_count>
MemoryPoolTempLate<block_size, block_count>::MemoryPoolTempLate() {
	const size_t min_size = sizeof(void*);
	_block_count = block_count;
	_block_size = (block_size / min_size) * min_size + (block_size % min_size ? min_size : 0);
}

MemoryManager::MemoryManager() {}

MemoryManager::~MemoryManager() {}

MemoryManager& MemoryManager::instance() {
	static MemoryManager mem_mgr;
	return mem_mgr;
}

void* MemoryManager::mmalloc(size_t size) {
	if (size < 32) return _mem32.mmalloc(size);
	else if (size < 64) return _mem64.mmalloc(size);
	else if (size < 128) return _mem128.mmalloc(size);
	else if (size < 256) return _mem256.mmalloc(size);
	else {
		MemoryBlock* block_res = (MemoryBlock*)malloc(size + sizeof(MemoryBlock));
		block_res->_id = -1;
		block_res->_ref = 1;
		block_res->_next = nullptr;
		block_res->_is_pool = false;
		block_res->_pool_ptr = nullptr;
		printf("[0]管理器申请内存 id<%d>, size<%d>, ptr<%llx>\n", block_res->_id, size, block_res);
		return (char*)block_res + sizeof(MemoryBlock);
	}
}

void MemoryManager::ffree(void* ptr) {
	MemoryBlock* block = (MemoryBlock*)((char*)ptr - sizeof(MemoryBlock));
	if (block->_is_pool) block->_pool_ptr->ffree(ptr);
	else if (--block->_ref == 0) {
		printf("[0]管理器释放内存 ptr<%llx>\n", block);
		free(block);
	}
}
