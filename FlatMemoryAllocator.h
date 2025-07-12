#pragma once

#include "IMemoryAllocator.h"
#include <vector>
#include <string>
#include <unordered_map>

class FlatMemoryAllocator : public IMemoryAllocator {
public:
    // Constructor and Destructor
    FlatMemoryAllocator(size_t maximumSize, size_t memPerFrame, size_t memPerProc);
    ~FlatMemoryAllocator();
    static void initialize(size_t maximumSize, size_t memPerFrame, size_t memPerProc);

    // Core interface overrides
    void* allocate(size_t size, string processName) override;
    void deallocate(void* ptr, string processName) override;
    std::string visualizeMemory() override;

    // Getters
    static FlatMemoryAllocator* getInstance();
    void logMemoryStateToFile(const std::string& filename);

private:
    // static variables
    static FlatMemoryAllocator* flatMemoryAllocator;

    // Member variables
    size_t memPerFrame;
    size_t memPerProc;
    int totalFrames;
    size_t maximumSize; // Max overall memory size
    size_t allocatedSize;
    std::vector<char> memory;        // Simulated memory ('.' = free, '#' = allocated)
    std::unordered_map<size_t, string> allocationMap; // True = allocated, False = free

    // Helper methods
    void initializeMemory(); // Initializes memory and allocation map
    bool canAllocateAt(size_t index, size_t size); // Checks if block can be allocated
    void allocateAt(size_t index, size_t size, string processName);    // Marks block as allocated
    void deallocateAt(size_t index, string processName);    
    
};

