#include "FlatMemoryAllocator.h"
#include <algorithm> // for std::fill

// Constructor
FlatMemoryAllocator::FlatMemoryAllocator(size_t maximumSize)
    : maximumSize(maximumSize), allocatedSize(0) {
    memory.resize(maximumSize, '.');
    allocationMap.resize(maximumSize, false);
    initializeMemory();
}

// Destructor
FlatMemoryAllocator::~FlatMemoryAllocator() {
    memory.clear();
    allocationMap.clear();
}

// Allocate memory block
void* FlatMemoryAllocator::allocate(size_t size) {
    for (size_t i = 0; i <= maximumSize - size; ++i) {
        if (!allocationMap[i] && canAllocateAt(i, size)) {
            allocateAt(i, size);
            return &memory[i];
        }
    }
    return nullptr;
}

// Deallocate memory block
void FlatMemoryAllocator::deallocate(void* ptr) {
    size_t index = static_cast<char*>(ptr) - &memory[0];
    if (allocationMap[index]) {
        deallocateAt(index);
    }
}

// Visualize memory
std::string FlatMemoryAllocator::visualizeMemory() {
    return std::string(memory.begin(), memory.end());
}

// Initialize memory
void FlatMemoryAllocator::initializeMemory() {
    std::fill(memory.begin(), memory.end(), '.');
    std::fill(allocationMap.begin(), allocationMap.end(), false);
}

// Check if a block can be allocated at index
bool FlatMemoryAllocator::canAllocateAt(size_t index, size_t size) {
    if (index + size > maximumSize) return false;
    for (size_t i = 0; i < size; ++i) {
        if (allocationMap[index + i]) return false;
    }
    return true;
}

// Allocate memory at index
void FlatMemoryAllocator::allocateAt(size_t index, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        memory[index + i] = '#';
        allocationMap[index + i] = true;
    }
    allocatedSize += size;
}

// Deallocate memory at index
void FlatMemoryAllocator::deallocateAt(size_t index) {
    while (index < maximumSize && allocationMap[index]) {
        memory[index] = '.';
        allocationMap[index] = false;
        ++index;
    }
}
