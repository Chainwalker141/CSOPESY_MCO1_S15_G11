#include "FlatMemoryAllocator.h"
#include <algorithm> // for std::fill
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <unordered_set>

using namespace std;

FlatMemoryAllocator* FlatMemoryAllocator::flatMemoryAllocator = nullptr;

// Constructor
FlatMemoryAllocator::FlatMemoryAllocator(size_t maximumSize, size_t memPerFrame)
    : maximumSize(maximumSize), memPerFrame(memPerFrame), allocatedSize(0) {
    
    initializeMemory();
}

// Destructor
FlatMemoryAllocator::~FlatMemoryAllocator() {
    memory.clear();
    allocationMap.clear();
}

void FlatMemoryAllocator::initialize(size_t maximumSize, size_t memPerFrame) {
    flatMemoryAllocator = new FlatMemoryAllocator(maximumSize, memPerFrame);
}

FlatMemoryAllocator* FlatMemoryAllocator::getInstance() {
    return flatMemoryAllocator;
}

int FlatMemoryAllocator::getTotalFrames() {
    return totalFrames;
}

void* FlatMemoryAllocator::getPointerToProcess(string processName) {
    for (size_t i = 0; i < maximumSize; ++i) {
        if (allocationMap[i] == processName) {
            return &memory[i];
        }
    }
    return nullptr;
}

bool FlatMemoryAllocator::isProcessActive(string processName) const {
    return activeProcesses.find(processName) != activeProcesses.end();
}

// Allocate memory block
void* FlatMemoryAllocator::allocate(size_t memSize, string processName) {
    // determine how many frames are needed
    size_t framesNeeded = (memSize + memPerFrame - 1) / memPerFrame;

    // check if pages needed exceeds number of frames available
    if (framesNeeded > freeFrameList.size()) {
        //std::cerr << "Memory Allocation Failed. Not Enough free Frames. \n";
        return nullptr;
    }

    // Find the first available block that can accommodate the process
    size_t remainingMemToAllocate = memSize;

    for (size_t i = 0; i < maximumSize; ++i) {
        if (allocationMap[i] == processName) {
            return &memory[i];
        }
        else {
            if (canAllocateAt(i, memSize, processName)) {
				cout << "Found available block at index: " << i << endl;
                allocateAt(i, memSize, processName);
            }
        }
    }

    // No available block found, return nullptr
    return nullptr;
}

// Deallocate memory block
void FlatMemoryAllocator::deallocate(void* ptr, string processName) {
    size_t index = static_cast<char*>(ptr) - &memory[0];
    if (allocationMap[index] == processName) {
        deallocateAt(index, processName);
        activeProcesses.erase(processName);
    }
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

    // initialize free frame list from frame 0 -> totalFrames-1
    for (size_t i = 0; i < static_cast<size_t>(totalFrames); ++i) {
        freeFrameList.push_back(i);
    }
}

// Check if a block can be allocated at index
bool FlatMemoryAllocator::canAllocateAt(size_t index, size_t memSize, string processName) {

    // returns false if process is already in memory (to prevent multiple allocation)
    if (activeProcesses.count(processName)) {
        return false;
    }

    if (index + memSize > maximumSize) return false;

    for (size_t i = 0; i < memSize; ++i) {
        size_t pos = index + i;
        auto it = allocationMap.find(pos);

        // If the key exists and is non-empty, it's already allocated
        if (it != allocationMap.end() && !it->second.empty()) {
            return false;
        }
    }
    return true;
}

// Allocate memory at frame
void FlatMemoryAllocator::allocateAt(size_t index, size_t memSize, string processName) {
    size_t frameIndex = freeFrameList.back();
    freeFrameList.pop_back();

    for (size_t i = 0; i < memSize; ++i) {
		memory[index + i] = '#'; // Mark as allocated
        allocationMap[index + i] = processName;
    }
    activeProcesses.insert(processName);
    allocatedSize += memSize;

	cout << "Allocated " << memSize << " bytes at frame " << frameIndex << " for process: " << processName << endl;
    cout << "Free frames: " << freeFrameList.size() << endl;
}

// Deallocate memory at index
void FlatMemoryAllocator::deallocateAt(size_t index, string processName) {

    cout << "Deallocating memory for process: " << processName << endl;
    //cout << "index: " << index << " is: " << memory[index] << " allocation map: " << allocationMap[index] << endl;
    while (index < maximumSize && (allocationMap[index] == processName)) {
        memory[index] = '.';
        allocationMap[index] = "";
        ++index;
    }
    //cout << "index: " << index-index << " is: " << memory[index-index] << " allocation map: " << allocationMap[index - index] << endl;
}

void FlatMemoryAllocator::logMemoryStateToFile(const std::string& filename) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening memory log file.\n";
        return;
    }

    // Timestamp
    std::time_t now = std::time(nullptr);
    std::tm tm;
    localtime_s(&tm, &now);  // Safe version on MSVC

    
 /*   if (file.is_open()) {*/
    file << "Timestamp: (" << std::put_time(&tm, "%m/%d/%Y %I:%M:%S%p") << ")\n";
        /*file.close();*/
    //}

    // Count number of processes in memory
    std::unordered_set<std::string> processes;
    for (const auto& [index, proc] : allocationMap) {
        if (!proc.empty()) processes.insert(proc);
    }

    file << "Number of processes in memory: " << processes.size() << "\n";

    // an extremely overcomplicated block of code which doesnt even work
    //// External fragmentation calculation (assume block = memPerFrame)
    //int externalFrag = 0;
    //int freeBlockSize = 0;

    //for (size_t i = 0; i < memory.size(); ++i) {
    //    if (memory[i] == '.') {
    //        freeBlockSize++;
    //    }
    //    else {
    //        if (freeBlockSize > 0 && freeBlockSize < memPerProc) {
    //            externalFrag += freeBlockSize;
    //        }
    //        freeBlockSize = 0;
    //    }
    //}

    //// Check at end of memory
    //if (freeBlockSize > 0 && freeBlockSize < memPerProc) {
    //    externalFrag += freeBlockSize;
    //}

    //file << "Total external fragmentation in KB: " << maximumSize-(processes.size()*memPerProc) << "\n\n";

    // Print layout (descending)
    file << "----end---- = " << maximumSize << "\n";

    std::string currentProcess = "";
    int endByte = -1;
    int startByte = -1;

    for (int i = static_cast<int>(memory.size()) - 1; i >= 0; --i) {
        std::string proc = allocationMap[i];
        if (proc == currentProcess) {
            startByte = i;
        } else {
            if (!currentProcess.empty()) {
                // Log the previous process block
                file << endByte << "\n";
                file << currentProcess << "\n";
                file << startByte << "\n\n";
            }

            // Start a new process block
            currentProcess = proc;
            if (!currentProcess.empty()) {
                endByte = i + 1;
                startByte = i;
            }
        }
    }

    // Edge case: first block
    if (!currentProcess.empty()) {
		file << endByte << "\n";
        file << currentProcess << "\n";
        file << startByte << endl;
    }

    file << "-----start----- = 0\n\n";
    file.close();
}
