#include "Scheduler.h"
#include <stdio.h>
#include <iostream>
#include "FlatMemoryAllocator.h"

using namespace std;

Scheduler* Scheduler::scheduler = nullptr;
int Scheduler::quantumCycle = 0; // remove after activity week 10


Scheduler::Scheduler(int numCores, bool isSchedulerRunning, int coresUsed, int coresAvailable, int timeQuantum, Scheduler::schedulingAlgorithm algo)
{
	this->numCores = numCores;
	this->isSchedulerRunning = isSchedulerRunning;
	this->coresUsed = coresUsed;
    this->coresAvailable = coresAvailable;
    this->timeQuantum = timeQuantum;
    this->algo = algo;
}

void Scheduler::initialize(int numCores, int timeQuantum, string schedulingAlgorithm) {
    if (schedulingAlgorithm == "rr") {
        scheduler = new Scheduler(numCores, false, 0, numCores, timeQuantum, RR);
    }
    else if (schedulingAlgorithm == "fcfs") {
        scheduler = new Scheduler(numCores, false, 0, numCores, timeQuantum, FCFS);
    }
}

Scheduler* Scheduler::getInstance() {
	return scheduler;
}

bool Scheduler::getIsSchedulerTestRunning()
{
	return this->isSchedulerTestRunning;
}

int Scheduler::getCoresUsed() {
    return this->coresUsed;
}

int Scheduler::getCoresAvailable() {
    return this->coresAvailable;
}

void Scheduler::setCoresUsed(int coresUsed) {
    this->coresUsed = coresUsed;
}

void Scheduler::setCoresAvailable(int coresAvailable) {
    this->coresAvailable = coresAvailable;
}

void Scheduler::setIsSchedulerTestRunning(bool isSchedulerTestRunning)
{
	this->isSchedulerTestRunning = isSchedulerTestRunning;
}

void Scheduler::assignProcess(std::shared_ptr<Console> console) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        processQueue.push(console);
    }
    queueCV.notify_one();  // Wake up a core thread
}

void Scheduler::rrScheduler(std::shared_ptr<Console> currentProcess, int coreId, void* memoryPtr) {
    bool processDoneFlag = false;
    
    // Log memory state to file after a quantum cycle
    FlatMemoryAllocator* flatMemoryInstance = FlatMemoryAllocator::getInstance();
    std::string filename = "memory_stamp_" + std::to_string(quantumCycle) + ".txt";
    flatMemoryInstance->logMemoryStateToFile(filename);

    if (memoryPtr) {
        for (int i = 0; i < this->timeQuantum; i++) {

            //currentProcess->setCurrentLine(currentProcess->getCurrentLine() + 1);

            //currentProcess->printFile(coreId); // TODO: FIX IMPLEMENTATION AFTER ACTIVITY

            currentProcess->runInstruction();
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // smaller number = faster processing time

            // Process is done but timeQuantum has not been finished
            if (currentProcess->getCurrentLine() == currentProcess->getTotalLine()) {
                processDoneFlag = true;
                break;
            }
        }
        static std::mutex quantumMutex;
        {
            std::lock_guard<std::mutex> lock(quantumMutex);
            quantumCycle++;
        }
    }
    else {
        //cout << "Process " << currentProcess->getProcessName() << " not in memory, skipping... \n" ;
    }

    this->coresUsed--; // TODO: MAKE SETTER
    this->coresAvailable++;

    if (processDoneFlag) {
        //cout << "\nFinished executing " << currentProcess->getProcessName() << endl;
		FlatMemoryAllocator* flatMemoryInstance = FlatMemoryAllocator::getInstance();
		flatMemoryInstance->deallocate(memoryPtr, currentProcess->getProcessName()); // Deallocate memory for the process
    }
    else {
		currentProcess->setCoreID(-1); // Reset Core ID for the process
        assignProcess(currentProcess); // Put Process back to the queue. TODO: MAYBE USE A DIFFERENT FUNCTION ?
    }
    
}

void Scheduler::fcfsScheduler(std::shared_ptr<Console> currentProcess, int coreId) {
    while (currentProcess->getCurrentLine() < currentProcess->getTotalLine()) {
        /*{
            static std::mutex coutMutex;
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "[Core " << coreId << "] ";
            currentProcess->printContents();
            std::cout << std::endl;
        }*/

        //currentProcess->setCurrentLine(currentProcess->getCurrentLine() + 1);
        currentProcess->runInstruction();
        //currentProcess->printFile(coreId); // TODO: FIX IMPLEMENTATION AFTER ACTIVITY

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // smaller number = faster processing time
    }
    // ENDOF FCFS

    //cout << "\nFinished executing " << currentProcess->getProcessName() << endl;
    this->coresUsed--; // TODO: MAKE SETTER
    this->coresAvailable++;
}

void Scheduler::start() {
    isSchedulerRunning = true;

    for (int coreId = 0; coreId < numCores; ++coreId) {
        coreThreads.emplace_back([this, coreId]() {
            while (isSchedulerRunning) {
                std::shared_ptr<Console> currentProcess = nullptr;
                void* allocatedMemory = nullptr;

                // READY QUEUE
                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    queueCV.wait(lock, [this]() {
                        return !processQueue.empty() || !isSchedulerRunning;
                    });

                    if (!isSchedulerRunning && processQueue.empty())
                        break;

                    currentProcess = processQueue.front();
                    processQueue.pop();
                }

                // Allocate memory before assigning to core
                FlatMemoryAllocator* flatMemoryInstance = FlatMemoryAllocator::getInstance();
                // check if process is already allocated memory
                if (flatMemoryInstance->isProcessActive(currentProcess->getProcessName())) { // get process from list
                    allocatedMemory = flatMemoryInstance->getPointerToProcess(currentProcess->getProcessName());
                }
                else { // allocate memory
                    allocatedMemory = flatMemoryInstance->allocate(
                        currentProcess->getMemSize(),
                        currentProcess->getProcessName(),
                        currentProcess
                    );
                }
                

                // check if memory has been allocated, if not, skip iteration and dont scheduler
                if (!allocatedMemory) {
                    /*std::cout << "[Core " << coreId << "] "
                              << "Failed to allocate memory for process "
                              << currentProcess->getProcessName() << ". Requeuing..." << std::endl;*/

                    // Optional: retry later by requeuing
                    {
                        std::lock_guard<std::mutex> lock(queueMutex);
                        processQueue.push(currentProcess);
                    }
                    queueCV.notify_all(); // wake other cores
                    continue; // skip this iteration
                }

                /*std::cout << "[Core " << coreId << "] "
                          << "Process " << currentProcess->getProcessName()
                          << " allocated " << currentProcess->getMemSize()
                          << " bytes in memory." << std::endl;*/

                currentProcess->setCoreID(coreId);

                // Update core tracking info
                this->coresUsed++;
                this->coresAvailable--;

                // Dispatch to appropriate scheduling function
                if (this->algo == FCFS) {
                    this->fcfsScheduler(currentProcess, coreId);
                } else if (this->algo == RR) {
                    this->rrScheduler(currentProcess, coreId, allocatedMemory);
                }

                //commented bc while logically it makes sense to return the cores used and available, it breaks the program for some reason
                //this->coresUsed--;
                //this->coresAvailable++;
            }
        });
    }
}