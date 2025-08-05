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
std::mutex FlatMemoryAllocator::backingStoreMutex;

FlatMemoryAllocator* FlatMemoryAllocator::flatMemoryAllocator = nullptr;

void FlatMemoryAllocator::printFrameMapContents() {
    std::cout << "\n=== Frame Map Contents ===\n";
    if (frameMap.empty()) {
        std::cout << "Frame map is empty.\n";
        return;
    }

    for (const auto& [frameIndex, info] : frameMap) {
        std::cout << "Frame " << frameIndex
            << " -> Process: " << info.processName
            << ", Page: " << info.pageNumber << "\n";
    }
    std::cout << "===========================\n";
}

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

const std::vector<size_t>& FlatMemoryAllocator::getFreeFrameList() const {
    return freeFrameList;
}

size_t FlatMemoryAllocator::getMemPerFrame() {
    return memPerFrame;
}

int FlatMemoryAllocator::getTotalFrames() {
    return totalFrames;
}

bool FlatMemoryAllocator::getPointerToProcess(string processName) {
    for (const auto& [frameIndex, info] : frameMap) {
        if (info.processName == processName && info.pageNumber == 0) {
            return true;
        }
    }

    // Fallback: return any frame belonging to the process if page 0 not found
    for (const auto& [frameIndex, info] : frameMap) {
        if (info.processName == processName) {
            return true;
        }
    }

    return false;
}

bool FlatMemoryAllocator::isProcessActive(string processName) const {
    return activeProcesses.find(processName) != activeProcesses.end();
}

// Allocate memory block
bool FlatMemoryAllocator::allocate(size_t memSize, string processName, shared_ptr<Console> Console) {
	//cout << "Allocating memory for process: " << processName << " with size: " << memSize << endl;

    // determine how many frames are needed
    size_t framesNeeded = (memSize + memPerFrame - 1) / memPerFrame;

    // check if pages needed exceeds number of frames available
    if (framesNeeded > freeFrameList.size()) {
        //std::cerr << "Memory Allocation Failed for process" << processName << "Not Enough free Frames. \n";
        return false;
    }

    // Find the first available block that can accommodate the process
    size_t bytesToAllocate = memSize;
    
    for (size_t pageToAllocate = 0; pageToAllocate < framesNeeded; pageToAllocate++) {

        size_t currentPageSize = std::min(bytesToAllocate, memPerFrame);

        //cout << "allocating page " << (pageToAllocate + 1) << " of " << framesNeeded << " for " << processName << endl;
        auto [startByte, endByte] = allocateAt(pageToAllocate, currentPageSize, processName, pageToAllocate);

        if (startByte == -1 && endByte == -1) { // no free frames are available
            cout << "Error allocating page " << pageToAllocate+1 << " for " << processName << endl;
        }
        else { // frames are available and have been set
            Console->setPageInfo(pageToAllocate, startByte, endByte, true);
        }
        

        bytesToAllocate -= currentPageSize;
    }

    activeProcesses.insert(processName);
    allocatedSize += memSize;

    // track how much memory was consumed
    VMStat::getInstance()->addUsedMemory(memPerFrame*framesNeeded);

    return true;
}

// Deallocate memory block
void FlatMemoryAllocator::deallocate(string processName) {
    //size_t index = static_cast<char*>(ptr) - &memory[0];
    //if (allocationMap[index] == processName) {
    //    deallocateAt(index, processName);
    //    activeProcesses.erase(processName);
    //}

    deallocateAt(0, processName);
    //activeProcesses.erase(processName);
}

// Deallocate one frame
void FlatMemoryAllocator::deallocateIndividualFrame(size_t frameIndex) {
    //lock guard
    /*std::lock_guard<std::mutex> lock(frameListMutex);*/

    if (frameQueue.empty()) {
        //std::cerr << "[OS Error] Frame queue is empty!\n";
        return;
    }

    if (frameMap.find(frameIndex) == frameMap.end()) {
        //std::cerr << "[OS Error] Tried to deallocate a non-existent frame: " << frameIndex << "\n";
        return;
    }

    std::string processName = frameMap[frameIndex].processName;  // save before erase
    size_t pageNumber = frameMap[frameIndex].pageNumber;

    freeFrameList.push_back(frameIndex);
    frameMap.erase(frameIndex);

    //std::cout << "[Deallocate] Frame " << frameIndex << " containing process " << processName << " page: " << pageNumber << " was deallocated.\n";
}

bool FlatMemoryAllocator::isPageLoaded(shared_ptr<Console> currentProcess) {
    std::shared_ptr<std::vector<PageInfo>> pageTablePtr = currentProcess->getPageTable();
    bool flag = true;

    /*for (const auto& [frameIndex, info] : frameMap) {
        if (info.processName == processName && info.pageNumber == pageNumber) {
            return true;
        }
    }*/
    if (!pageTablePtr) {
        return false;
    }

    for (const auto& page : *pageTablePtr) {
        
        if (page.valid) {
			continue; // if page is valid, it is already loaded
        }
        else {
            flag = false;
        }
        
      
	}

    return flag;
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

// Allocate memory at frame
pair<size_t, size_t> FlatMemoryAllocator::allocateAt(size_t index, size_t bytesToAllocate, string processName, size_t pageNum) {
    size_t frameIndex;
    { // mutex just in case it gets called again
        std::lock_guard<std::mutex> lock(frameListMutex);

        if (freeFrameList.empty()) {
            // std::cerr << "[OS Error] I lied haha, no frames available for: " << processName << " page: " << pageNum << endl;
            return { -1, -1 }; // no allocation
        }

        frameIndex = freeFrameList.back();
        freeFrameList.pop_back();
        frameQueue.push(frameIndex); // Add frame to queue for tracking
    }

  //  for (size_t i = 0; i < memSize; ++i) {
		//memory[index + i] = '#'; // Mark as allocated
  //      allocationMap[index + i] = processName;
  //  }

    size_t startByte = frameIndex * memPerFrame;
    size_t endByte = startByte + bytesToAllocate - 1;

    for (size_t i = 0; i < bytesToAllocate; ++i) {
        allocationMap[startByte + i] = processName;
    }

    // Inserting into frameMap
    FrameInfo info;
    info.processName = processName;
    info.pageNumber = pageNum;

    frameMap[frameIndex] = info;


	// cout << "Allocated frame " << frameIndex << " with page: " << pageNum << " for process: " << processName << endl;

    // NOTE: we may have to add lock guard here
 /*   processPageTable[processName].insert(pageNum);*/

	//cout << "Allocated frame " << frameIndex << " with page: " << pageNum+1 << " for process: " << processName << endl;

    //cout << "Free frames: " << freeFrameList.size() << endl;
    return { startByte, endByte };
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
    //lock guard
	std::lock_guard<std::mutex> lock(frameListMutex);

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
        // track how much was freed
        VMStat::getInstance()->subUsedMemory(memPerFrame);
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

void FlatMemoryAllocator::writePageToBackingStore(FrameInfo victimFrame) {
    std::lock_guard<std::mutex> lock(backingStoreMutex);

    std::ofstream outfile("csopesy-backing-store.txt", std::ios::app); // append
    if (!outfile.is_open()) {
        std::cerr << "Error writing to backing store.\n";
        return;
    }

    outfile << "PROCESS " << victimFrame.processName << "\n";
	outfile << "PAGE " << victimFrame.pageNumber << "\n";


    //outfile << "ENDPROCESS\n\n"; // Clear separation for parsing later
    outfile.close();
    VMStat::getInstance()->incrementPagedOut();
}

void FlatMemoryAllocator::loadPageFromBackingStore(std::string processName, std::shared_ptr<Console> console) {
    std::lock_guard<std::mutex> lock(backingStoreMutex);

    std::ifstream infile("csopesy-backing-store.txt");
    if (!infile.is_open()) {
        std::cerr << "Error opening backing store.\n";
        return;
    }

    std::string line;
    std::vector<std::string> allLines;
    std::vector<int> pageNumbers;
    bool found = false;

    // Read file into memory
    while (std::getline(infile, line)) {
        allLines.push_back(line);
    }
    infile.close();

    // Prepare to filter out lines related to the process
    std::vector<std::string> updatedLines;
    for (size_t i = 0; i < allLines.size(); ++i) {
        if (allLines[i] == "PROCESS " + processName) {
            size_t j = i + 1;
            while (j < allLines.size() && allLines[j].rfind("PAGE ", 0) == 0) {
                int pageNumber = std::stoi(allLines[j].substr(5));
                pageNumbers.push_back(pageNumber);
                ++j;
            }
            found = true;
            i = j - 1;  // Skip these lines (PROCESS and its PAGEs)
        }
        else {
            updatedLines.push_back(allLines[i]);
        }
    }

    if (!found) {
        //std::cerr << "[DEBUG] No pages found in backing store for process: " << processName << std::endl;
        return;
    }

    // Allocate the recovered pages
    for (int pageNum : pageNumbers) {
        //std::cout << "[LOAD] Loading page " << pageNum << " for process " << processName << std::endl;

        size_t dummySize = std::min(console->getMemSize(), (size_t)memPerFrame);
        auto [startByte, endByte] = allocateAt(pageNum, dummySize, processName, pageNum);

        if (startByte == (size_t)-1 && endByte == (size_t)-1) {
            std::cerr << "[LOAD ERROR] Failed to allocate frame for page " << pageNum << " of process " << processName << std::endl;
        }
        else {
            VMStat::getInstance()->incrementPagedIn();
            console->setPageInfo(pageNum, startByte, endByte, true);
        }
    }

    // Rewrite the file without the loaded process/pages
    std::ofstream outfile("csopesy-backing-store.txt", std::ios::trunc);
    if (!outfile.is_open()) {
        std::cerr << "Error rewriting backing store.\n";
        return;
    }

    for (const std::string& remainingLine : updatedLines) {
        outfile << remainingLine << "\n";
    }

    outfile.close();
}


std::string FlatMemoryAllocator::evictOneProcessToBackingStore(shared_ptr<Console> currentProcess) {
	std::lock_guard<std::mutex> lock(frameListMutex);
    //printFrameMapContents();
    if (frameQueue.empty()) {
        std::cerr << "[OS Warning] No frames to evict frameQueue is empty.\n";
        return "";
    }

    // Evict the first process in the set (FIFO style eviction)
    size_t victimFrameIndex = frameQueue.front();
    frameQueue.pop();

    auto it = frameMap.find(victimFrameIndex);
    if (it == frameMap.end()) {
        std::cerr << "[OS Warning] Frame " << victimFrameIndex << " not found in frameMap.\n";
        return "";
    }

    FrameInfo victimInfo = it->second;
    std::string victimProcess = it->second.processName;

    auto victimScreen = ConsoleManager::getInstance()->getScreenMap()[victimProcess];

	cout << "Evicting Frame: " << victimFrameIndex << " to backing store." << endl;
	writePageToBackingStore(victimInfo);

	deallocateIndividualFrame(victimFrameIndex); // Deallocate the first frame of the victim process

    victimScreen->setPageInfo(victimInfo.pageNumber, -1, -1, false); // Mark the page as not loaded

	return victimProcess; // Return the name of the evicted process
}




