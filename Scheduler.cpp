#include "Scheduler.h"
#include <stdio.h>
#include <iostream>
#include <mutex>


using namespace std;

Scheduler* Scheduler::scheduler = nullptr;


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

void Scheduler::rrScheduler(std::shared_ptr<Console> currentProcess, int coreId) {
    bool processDoneFlag = false;
    
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
    this->coresUsed--; // TODO: MAKE SETTER
    this->coresAvailable++;

    if (processDoneFlag) {
        //cout << "\nFinished executing " << currentProcess->getProcessName() << endl;
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
                    this->coresUsed++; // TODO: MAKE SETTER
                    this->coresAvailable--;
                }

                if (this->algo == FCFS) {
                    this->fcfsScheduler(currentProcess, coreId);
                }
                else if (this->algo == RR) {
                    this->rrScheduler(currentProcess, coreId);
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