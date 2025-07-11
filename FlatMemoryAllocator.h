#pragma once

#include "IMemoryAllocator.h"
#include <vector>
#include <string>

class FlatMemoryAllocator : public IMemoryAllocator {
public:
    // Constructor and Destructor
    FlatMemoryAllocator(size_t maximumSize);
    ~FlatMemoryAllocator();

    // Core interface overrides
    void* allocate(size_t size) override;
    void deallocate(void* ptr) override;
    std::string visualizeMemory() override;

private:
    // Member variables
    size_t maximumSize;
    size_t allocatedSize;
    std::vector<char> memory;        // Simulated memory ('.' = free, '#' = allocated)
    std::vector<bool> allocationMap; // True = allocated, False = free

    // Helper methods
    void initializeMemory(); // Initializes memory and allocation map
    bool canAllocateAt(size_t index, size_t size); // Checks if block can be allocated
    void allocateAt(size_t index, size_t size);    // Marks block as allocated
    void deallocateAt(size_t index);               // Frees block starting at index
};
