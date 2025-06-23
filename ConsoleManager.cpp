#include "ConsoleManager.h"
#include <iostream>
#include "Scheduler.h"
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <random>
#include "DeclareCommand.h"
#include "AddCommand.h"
#include "SubCommand.h"

using namespace std;

ConsoleManager* ConsoleManager::consoleManager = nullptr;

ConsoleManager::ConsoleManager() {
};

void ConsoleManager::initialize() {
    consoleManager = new ConsoleManager();
}

ConsoleManager* ConsoleManager::getInstance()
{
    return consoleManager;
}

void ConsoleManager::registerConsole(shared_ptr<Console> screenRef) {
    this->screenMap[screenRef->getProcessName()] = screenRef;
}

void ConsoleManager::drawConsole(string processName) {
    auto screen = screenMap.find(processName);
    if (screen != screenMap.end()) {
        this->screenMap[processName]->printContents();
    }
    else {
        cout << "screen not found\n";
    }
}

bool ConsoleManager::screenExists(string processName) {
    return screenMap.find(processName) != screenMap.end();
}

void ConsoleManager::setInitialize(bool isInit)
{
    this->isInit = isInit;
}

bool ConsoleManager::getInitialize() {
    return this->isInit;
}

int ConsoleManager::getMaxIns() {
    return this->maxIns;
}

void ConsoleManager::setMaxIns(int maxIns) {
    this->maxIns = maxIns;
}

int ConsoleManager::getMinIns() {
    return this->minIns;
}

void ConsoleManager::setMinIns(int minIns) {
    this->minIns = minIns;
}

std::string ConsoleManager::getCurrentTimeStamp() {
    std::time_t now = std::time(nullptr);
    std::tm localTime;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime); 
#endif

    std::ostringstream oss;
    oss << std::put_time(&localTime, "%m/%d/%Y ");

    int hour = localTime.tm_hour;
    bool isPM = hour >= 12;
    if (hour == 0) hour = 12;
    else if (hour > 12) hour -= 12;

    oss << hour << ':'
        << std::setw(2) << std::setfill('0') << localTime.tm_min << ':'
        << std::setw(2) << std::setfill('0') << localTime.tm_sec
        << (isPM ? "PM" : "AM");

    return oss.str();
}

void ConsoleManager::schedulerTest(int NUM_PROCESSES) {
    static int process_counter = 0;
    while (Scheduler::getInstance()->getIsSchedulerTestRunning()) {
        for (int i = 0; i < NUM_PROCESSES; i++) {
            //cout << "Creating Process " << process_counter << endl;
            process_counter++;
            string processName = "P" + std::to_string(process_counter);

            //int totalIns = 100;
            int totalIns = generateRandInt(minIns, maxIns); // Generate random instruction total
            
            shared_ptr<Console> processConsole = make_shared<Console>(
                processName, 0, totalIns, ConsoleManager::getInstance()->getCurrentTimeStamp()); // creates a process "P(N)" which has 10 lines and created at a certain time

            ConsoleManager::getInstance()->generateCommands(processConsole);
            ConsoleManager::getInstance()->registerConsole(processConsole);
            Scheduler::getInstance()->assignProcess(processConsole);

            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // delay in process creation
        }

    }
    
}

int ConsoleManager::generateRandInt(int minIns, int maxIns) {
    static std::random_device rd;   
    static std::mt19937 gen(rd());    
    std::uniform_int_distribution<> distrib(minIns, maxIns);
    return distrib(gen);
}

void ConsoleManager::generateCommands(std::shared_ptr<Console> process) {
    std::queue<std::shared_ptr<ICommand>> commandList;
    int totalIns = process->getTotalLine();
    for (int i = 0; i < totalIns; i++) {
        // TODO: MAKE RANDOM PER ITERATION 
        //commandList.push(make_shared<DeclareCommand>(process->getProcessName(), "val", 430, process->getVarTable())); // DECLARE
        //commandList.push(make_shared<AddCommand>(process->getProcessName(), "sum", 3, "val", process->getVarTable())); // ADD
        commandList.push(make_shared<SubCommand>(process->getProcessName(), "diff", "val", 100, process->getVarTable())); // SUBTRACT
    }
    process->setCommandList(commandList);
}

unordered_map<string, shared_ptr<Console>> ConsoleManager::getScreenMap() {
    return this->screenMap;
}

void ConsoleManager::displayProcessSmi() {
    unordered_map<string, shared_ptr<Console>> screenMap = ConsoleManager::getInstance()->getScreenMap();
    Scheduler* scheduler = Scheduler::getInstance();
    int coresUsed = scheduler->getCoresUsed(); // TODO: change(hardcoded)
    int coresAvailable = scheduler->getCoresAvailable();
    float cpuUtilization = (float)coresUsed / (coresUsed + coresAvailable) * 100;

    cout << "\nCPU Utilization: " << cpuUtilization << "%" << endl;
    cout << "Cores used: " << coresUsed << endl;
    cout << "Cores available: " << coresAvailable << endl;
    cout << "-----------------------------------" << endl;
    cout << "Running processes:" << endl;
    for (const auto& pair : screenMap) {
        shared_ptr<Console> screenPtr = pair.second;



        auto coreID = screenPtr->getProcessName();
        string coreIDstr;
        /*   if (coreID == -1) {
               coreIDstr = "N/A";
           }
           else {
               coreIDstr = to_string(coreID);
           }*/

        cout << "Name: " << screenPtr->getProcessName() << " | "
            << screenPtr->getTimestamp() << " | "
            << "Core: " << coreIDstr << " | "
            << screenPtr->getCurrentLine() << "/"
            << screenPtr->getTotalLine() << " | " << endl;

    }
}

