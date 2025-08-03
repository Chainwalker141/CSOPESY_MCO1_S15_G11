#include "FlatMemoryAllocator.h"
#include <algorithm> // for std::fill
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <unordered_set>
#include <mutex>

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
    for (const auto& [frameIndex, info] : frameMap) {
        if (info.processName == processName && info.pageNumber == 0) {
            return &memory[frameIndex * memPerFrame];
        }
    }

    // Fallback: return any frame belonging to the process if page 0 not found
    for (const auto& [frameIndex, info] : frameMap) {
        if (info.processName == processName) {
            return &memory[frameIndex * memPerFrame];
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
    size_t pageToAllocate = 0;

    /*for (size_t i = 0; i < maximumSize; ++i) {
        if (allocationMap[i] == processName) {
            return &memory[i];
        }
        else {
            if (canAllocateAt(i, memSize, processName)) {
				cout << "Found available block at index: " << i << endl;
                allocateAt(i, memSize, processName, pageToAllocate);
                ++pageToAllocate;
            }
        }
    }*/

    for (size_t i = 0; i < framesNeeded; i++) {
        cout << "allocating page " << i+1 << " of " << framesNeeded << " for " << processName << endl;
        allocateAt(i, memSize, processName, pageToAllocate);
        ++pageToAllocate;
    }

    activeProcesses.insert(processName);
    allocatedSize += memSize;

    // No available block found, return nullptr
    return nullptr;
}

// Deallocate memory block
void FlatMemoryAllocator::deallocate(void* ptr, string processName) {
    //size_t index = static_cast<char*>(ptr) - &memory[0];
    //if (allocationMap[index] == processName) {
    //    deallocateAt(index, processName);
    //    activeProcesses.erase(processName);
    //}

    deallocateAt(0, processName);
    //    activeProcesses.erase(processName);
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
void FlatMemoryAllocator::allocateAt(size_t index, size_t memSize, string processName, size_t pageNum) {
    size_t frameIndex;
    { // mutex just in case it gets called again
        std::lock_guard<std::mutex> lock(frameListMutex);

        if (freeFrameList.empty()) {
            std::cerr << "[OS Error] I lied haha, no frames available for: " << processName << " page: " << pageNum << endl;
            return;
        }

        frameIndex = freeFrameList.back();
        freeFrameList.pop_back();
    }

  //  for (size_t i = 0; i < memSize; ++i) {
		//memory[index + i] = '#'; // Mark as allocated
  //      allocationMap[index + i] = processName;
  //  }

    // Inserting into frameMap
    FrameInfo info;
    info.processName = processName;
    info.pageNumber = pageNum; // TODO: CHANGE

    frameMap[frameIndex] = info;

	cout << "Allocated frame " << frameIndex << " with page: " << pageNum << " for process: " << processName << endl;
    //cout << "Free frames: " << freeFrameList.size() << endl;
}

// Deallocate memory at index
void FlatMemoryAllocator::deallocateAt(size_t index, string processName) {

    //cout << "Deallocating memory for process: " << processName << endl;
    //cout << "index: " << index << " is: " << memory[index] << " allocation map: " << allocationMap[index] << endl;
    /*while (index < maximumSize && (allocationMap[index] == processName)) {
        memory[index] = '.';
        allocationMap[index] = "";
        ++index;
    }*/
    //cout << "index: " << index-index << " is: " << memory[index-index] << " allocation map: " << allocationMap[index - index] << endl;

    std::vector<size_t> framesToFree; // to determine which frames are to be freed up

    // Find all frames that belong to the process
    for (const auto& [frameIndex, info] : frameMap) {
        if (info.processName == processName) {
            framesToFree.push_back(frameIndex);
        }
    }

    // Free it up
    for (size_t frameIndex : framesToFree) {
        // Return frame to freeFrameList and remove from frameMap
        freeFrameList.push_back(frameIndex);
        frameMap.erase(frameIndex);
    }

    //cout << "Deallocated " << framesToFree.size() << " frame(s) for process: " << processName << endl;
    //cout << "Free frames: " << freeFrameList.size() << endl;
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

void FlatMemoryAllocator::writePageToBackingStore(string processName) {
    ofstream outfile("csopesy-backing-store.txt", ios::app); // append
    if (!outfile.is_open()) {
        cerr << "Error writing to backing store.\n";
        return;
    }

    outfile << processName << "\n";
    for (size_t i = 0; i < maximumSize; ++i) {
        if (allocationMap[i] == processName) {
            outfile << memory[i];
        }
    }
    outfile << "\n\n"; // blank line to separate entries
    outfile.close();
}

void FlatMemoryAllocator::loadPageFromBackingStore(string processName) {
    ifstream infile("csopesy-backing-store.txt");
    if (!infile.is_open()) {
        cerr << "Error opening backing store.\n";
        return;
    }

    string line, content;
    bool found = false;
    while (getline(infile, line)) {
        if (line == processName) {
            found = true;
            string data;
            while (getline(infile, data) && !data.empty()) {
                content += data;
            }
            break;
        }
    }
    infile.close();

    if (found) {
        void* ptr = allocate(content.size(), processName);
        if (ptr != nullptr) {
            for (size_t i = 0; i < content.size(); ++i) {
                memory[static_cast<char*>(ptr) - &memory[0] + i] = content[i];
            }
        }
    }
}


