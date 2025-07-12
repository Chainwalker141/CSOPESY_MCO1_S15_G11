#include "Scheduler.h"
#include <stdio.h>
#include <iostream>
#include <mutex>
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

        std::this_thread::sleep_for(std::chrono::seconds(1)); // smaller number = faster processing time
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
                //std::cout << isSchedulerRunning;
                std::shared_ptr<Console> currentProcess = nullptr;
                void* allocatedMemory = nullptr;

                // Wait for process from shared queue
                // READY QUEUE TOH
                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    queueCV.wait(lock, [this]() {
                        return !processQueue.empty() || !isSchedulerRunning;
                        });

					// Ensures that core has something to process and scheduler is stil running
                    if (!isSchedulerRunning && processQueue.empty())
                        break;

                    currentProcess = processQueue.front();
					currentProcess->setCoreID(coreId); // Set Core ID for the process

                    processQueue.pop();

                    // Allocate a process in the memory
					// NOTE: remove 100. There is already mem-per-proc that is set inhe FlatMemoryAllocator
					FlatMemoryAllocator* flatMemoryInstance = FlatMemoryAllocator::getInstance();
                    
					allocatedMemory = flatMemoryInstance->allocate(100, currentProcess->getProcessName());

                    // FOR DEBUGGING
                    if (allocatedMemory) {
						//cout << "Process " << currentProcess->getProcessName() << " allocated in memory." << endl;
                        
                    }
                    else {
                        //cout << "Failed to allocate memory for process " << currentProcess->getProcessName() << ". Skipping..." << endl;
						//processQueue.push(currentProcess); // Re-add process to the queue
                    }



                    this->coresUsed++; // TODO: MAKE SETTER
                    this->coresAvailable--;
                }

                if (this->algo == FCFS) {
                    this->fcfsScheduler(currentProcess, coreId);
                }
                else if (this->algo == RR) {
                    this->rrScheduler(currentProcess, coreId, allocatedMemory);
                }
                //// FCFS 
                //// Process line-by-line
                //while (currentProcess->getCurrentLine() < currentProcess->getTotalLine()) {
                //    /*{
                //        static std::mutex coutMutex;
                //        std::lock_guard<std::mutex> lock(coutMutex);
                //        std::cout << "[Core " << coreId << "] ";
                //        currentProcess->printContents();
                //        std::cout << std::endl;
                //    }*/

                //    currentProcess->setCurrentLine(currentProcess->getCurrentLine() + 1);

                //    currentProcess->printFile(coreId); // TODO: FIX IMPLEMENTATION AFTER ACTIVITY
                //    
                //    std::this_thread::sleep_for(std::chrono::seconds(1)); // smaller number = faster processing time
                //}
                //// ENDOF FCFS

                //cout << "\nFinished executing " << currentProcess->getProcessName() << endl;
                //this->coresUsed--; // TODO: MAKE SETTER
                //this->coresAvailable++;
            }
            });
    }
}