#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <memory>
#include "Console.h"

class Scheduler {
private:
    enum schedulingAlgorithm { RR, FCFS }; // RR Implementation
    schedulingAlgorithm algo;

    int numCores;
    bool isSchedulerRunning;
    bool isSchedulerTestRunning;
    int coresUsed;
    int coresAvailable;
    int timeQuantum; // RR Implementation

    static Scheduler* scheduler;
    std::vector<std::thread> coreThreads;

    std::queue<std::shared_ptr<Console>> processQueue;
    std::mutex queueMutex;
    std::condition_variable queueCV;

    void fcfsScheduler(std::shared_ptr<Console> currentProcess, int coreId);
    void rrScheduler(std::shared_ptr<Console> currentProcess, int coreId);

public:
    Scheduler(int numCores, bool isSchedulerRunning, int coresUsed, int coresAvailable, int timeQuantum, Scheduler::schedulingAlgorithm algo);
    static void initialize(int numCores, int timeQuantum, string schedulingAlgorithm);
    static Scheduler* getInstance();

    void start();
    void assignProcess(std::shared_ptr<Console> console);
    bool getIsSchedulerTestRunning();
    int getCoresUsed();
    int getCoresAvailable();
    void setIsSchedulerTestRunning(bool isSchedulerTestRunning);
	void setCoresUsed(int coresUsed);
    void setCoresAvailable(int coresAvailable);
};
