#include "FlatMemoryAllocator.h"
#include <algorithm> // for std::fill
#include <iostream>
#include <string>

using namespace std;

FlatMemoryAllocator* FlatMemoryAllocator::flatMemoryAllocator = nullptr;

// Constructor
FlatMemoryAllocator::FlatMemoryAllocator(size_t maximumSize, size_t memPerFrame, size_t memPerProc)
    : maximumSize(maximumSize), memPerFrame(memPerFrame), memPerProc(memPerProc), allocatedSize(0) {
    
    initializeMemory();
}

// Destructor
FlatMemoryAllocator::~FlatMemoryAllocator() {
    memory.clear();
    allocationMap.clear();
}

void FlatMemoryAllocator::initialize(size_t maximumSize, size_t memPerFrame, size_t memPerProc) {
    flatMemoryAllocator = new FlatMemoryAllocator(maximumSize, memPerFrame, memPerProc);
}

FlatMemoryAllocator* FlatMemoryAllocator::getInstance() {
    return flatMemoryAllocator;
}

// Allocate memory block
void* FlatMemoryAllocator::allocate(size_t size, string processName) {
    // Find the first available block that can accommodate the process
    int freeFramesFound = 0;
    int startIndex = -1;

    int framesPerProc = static_cast<int>(memPerProc);

	cout << "Allocating " << size << " bytes for process: " << processName << endl;
	cout << "Frames per process: " << framesPerProc << endl;

    for (size_t i = 0; i < maximumSize; ++i) {
        if (allocationMap[i] == processName) {
            return &memory[i];
        }
        else {
            if (canAllocateAt(i, memPerProc)) {
				cout << "Found available block at index: " << i << endl;
                allocateAt(i, memPerProc, processName);
            }
            else {
				cout << "Block at index " << i << " is not available for allocation." << endl;
            }
        }
    }

    // No available block found, return nullptr
    return nullptr;
}

// Deallocate memory block
void FlatMemoryAllocator::deallocate(void* ptr) {
    size_t index = static_cast<char*>(ptr) - &memory[0];
    /*if (allocationMap[index]) {
        deallocateAt(index);
    }*/
}

// Visualize memory
std::string FlatMemoryAllocator::visualizeMemory() {
    return std::string(memory.begin(), memory.end());
}

// Initialize memory
void FlatMemoryAllocator::initializeMemory() {
    totalFrames = static_cast<int>(maximumSize / memPerFrame);

    memory.resize(maximumSize, '.');          
    allocationMap.clear();                     // just in case
    for (size_t i = 0; i < maximumSize; ++i) {
        allocationMap[i] = "";                 // "" = free
    }
}

// Check if a block can be allocated at index
bool FlatMemoryAllocator::canAllocateAt(size_t index, size_t size) {
    if (index + size > maximumSize) return false;

    for (size_t i = 0; i < size; ++i) {
        size_t pos = index + i;
        auto it = allocationMap.find(pos);

        // If the key exists and is non-empty, it's already allocated
        if (it != allocationMap.end() && !it->second.empty()) {
            return false;
        }
    }
    return true;
}

// Allocate memory at index
void FlatMemoryAllocator::allocateAt(size_t index, size_t size, string processName) {
    /*for (size_t i = 0; i < size; ++i) {
        memory[index + i] = '.';
        allocationMap[index + i] = true;
    }
    allocatedSize += size;*/
    for (size_t i = 0; i < memPerProc; ++i) {
		memory[index + i] = '#'; // Mark as allocated
        allocationMap[index + i] = processName;
    }
    allocatedSize += size;

	cout << "Allocated " << size << " bytes at index " << index << " for process: " << processName << endl;
}

// Deallocate memory at index
//void FlatMemoryAllocator::deallocateAt(size_t index) {
//    while (index < maximumSize && allocationMap[index]) {
//        memory[index] = '#';
//        allocationMap[index] = false;
//        ++index;
//    }
//}
