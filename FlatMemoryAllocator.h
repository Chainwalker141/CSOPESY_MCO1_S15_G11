#pragma once

#include "IMemoryAllocator.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include "Console.h"

// Struct to store process name and page number in each frame
struct FrameInfo {
    std::string processName;
    size_t pageNumber;
};

class FlatMemoryAllocator : public IMemoryAllocator {
public:
    // Constructor and Destructor
    FlatMemoryAllocator(size_t maximumSize, size_t memPerFrame);
    ~FlatMemoryAllocator();
    static void initialize(size_t maximumSize, size_t memPerFrame);

    // Core interface overrides
    void* allocate(size_t size, string processName, shared_ptr<Console> Console) override;
    void deallocate(void* ptr, string processName) override;
    std::string visualizeMemory() override;

    void* getPointerToProcess(string processName);

    // Getters
    static FlatMemoryAllocator* getInstance();
    int getTotalFrames();
    void logMemoryStateToFile(const std::string& filename);
    void writePageToBackingStore(string processName);
    void loadPageFromBackingStore(string processName);
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
    std::unordered_map<size_t, string> allocationMap; // where each proc is stored in each individual byte in memory
    std::unordered_map <size_t, FrameInfo> frameMap; // FRAME MAP IN RAM, key is frame num, value is the Frame Info which contains the procName and its page
    std::vector<size_t> freeFrameList; // TO DETERMINE WHICH FRAMES ARE UNOCCUPIED
    std::unordered_set<std::string> activeProcesses;

    std::mutex frameListMutex;

    // Helper methods
    void initializeMemory(); // Initializes memory and allocation map
    bool canAllocateAt(size_t index, size_t size, string processName); // Checks if block can be allocated
    pair<size_t, size_t> allocateAt(size_t index, size_t bytesToAllocate, string processName, size_t pageNum);    // Marks block as allocated
    void deallocateAt(size_t index, string processName);   
    
};

