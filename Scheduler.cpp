#include "Scheduler.h"
#include <stdio.h>
#include <iostream>
#include <mutex>

using namespace std;

Scheduler* Scheduler::scheduler = nullptr;


Scheduler::Scheduler(int numCores, bool isSchedulerRunning, int coresUsed, int coresAvailable)
{
	this->numCores = numCores;
	this->isSchedulerRunning = isSchedulerRunning;
	this->coresUsed = coresUsed;
	this->coresAvailable = coresAvailable;
}

void Scheduler::initialize(int numCores) {
	scheduler = new Scheduler(numCores, false, 0, numCores);
}

Scheduler* Scheduler::getInstance() {
	return scheduler;
}

bool Scheduler::getIsSchedulerTestRunning()
{
	return this->isSchedulerTestRunning;
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


void Scheduler::start() {
    isSchedulerRunning = true;

    for (int coreId = 0; coreId < numCores; ++coreId) {
        coreThreads.emplace_back([this, coreId]() {
            while (isSchedulerRunning) {
                std::shared_ptr<Console> currentProcess = nullptr;

                // Wait for process from shared queue
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

                // Process line-by-line
                while (currentProcess->getCurrentLine() < currentProcess->getTotalLine()) {
                    /*{
                        static std::mutex coutMutex;
                        std::lock_guard<std::mutex> lock(coutMutex);
                        std::cout << "[Core " << coreId << "] ";
                        currentProcess->printContents();
                        std::cout << std::endl;
                    }*/

                    currentProcess->setCurrentLine(currentProcess->getCurrentLine() + 1);
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }
            });
    }
}


