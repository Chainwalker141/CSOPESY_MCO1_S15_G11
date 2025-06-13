#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <memory>
#include "Console.h"

class Scheduler {
private:
    int numCores;
    bool isSchedulerRunning;
    bool isSchedulerTestRunning;
    int coresUsed;
    int coresAvailable;

    static Scheduler* scheduler;
    std::vector<std::thread> coreThreads;

    std::queue<std::shared_ptr<Console>> processQueue;
    std::mutex queueMutex;
    std::condition_variable queueCV;

public:
    Scheduler(int numCores, bool isSchedulerRunning, int coresUsed, int coresAvailable);
    static void initialize(int numCores);
    static Scheduler* getInstance();

    void start();
    void assignProcess(std::shared_ptr<Console> console);
    bool getIsSchedulerTestRunning();
    void setIsSchedulerTestRunning(bool isSchedulerTestRunning);
};
