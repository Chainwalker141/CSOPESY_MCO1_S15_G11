#pragma once
#include <string>
#include "Console.h"

using namespace std;

class IMemoryAllocator {
public:
	virtual void* allocate(size_t size, string processName, shared_ptr<Console> Console) = 0;
	virtual void deallocate(void* ptr, string processName) = 0;
	virtual std::string visualizeMemory() = 0;
};