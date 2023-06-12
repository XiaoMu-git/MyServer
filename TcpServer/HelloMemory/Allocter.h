#ifndef _Allocter_h_
#define _Allocter_h_

#include "MemoryPool.h"

void* operator new (size_t size);

void operator delete (void* pointer);

void* operator new[](size_t size);

void operator delete[] (void* pointer);

#endif // !_Allocter_h_


