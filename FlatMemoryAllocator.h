#pragma once

#include "IMemoryAllocator.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

class FlatMemoryAllocator : public IMemoryAllocator {
public:
    // Constructor and Destructor
    FlatMemoryAllocator(size_t maximumSize, size_t memPerFrame);
    ~FlatMemoryAllocator();
    static void initialize(size_t maximumSize, size_t memPerFrame);

    // Core interface overrides
    void* allocate(size_t size, string processName) override;
    void deallocate(void* ptr, string processName) override;
    std::string visualizeMemory() override;

    void* getPointerToProcess(string processName);

    // Getters
    static FlatMemoryAllocator* getInstance();
    int getTotalFrames();
    void logMemoryStateToFile(const std::string& filename);
    bool isProcessActive(string processName) const;

private:
    // static variables
    static FlatMemoryAllocator* flatMemoryAllocator;

    // Member variables
    size_t memPerFrame;
    int totalFrames;
    size_t maximumSize; // Max overall memory size
    size_t allocatedSize;
    std::vector<char> memory;        // Simulated memory ('.' = free, '#' = allocated)
    std::unordered_map<size_t, string> allocationMap; // THE PAGE TABLE, size_t being the frame, string being the process' PAGE in that frame
    std::vector<size_t> freeFrameList; // TO DETERMINE WHICH FRAMES ARE UNOCCUPIED
    std::unordered_set<std::string> activeProcesses;

    // Helper methods
    void initializeMemory(); // Initializes memory and allocation map
    bool canAllocateAt(size_t index, size_t size, string processName); // Checks if block can be allocated
    void allocateAt(size_t index, size_t size, string processName);    // Marks block as allocated
    void deallocateAt(size_t index, string processName);   
    
};

