#pragma once
#include <string>

using namespace std;

class IMemoryAllocator {
public:
	virtual void* allocate(size_t size, string processName) = 0;
	virtual void deallocate(void* ptr, string processName) = 0;
	virtual std::string visualizeMemory() = 0;
};