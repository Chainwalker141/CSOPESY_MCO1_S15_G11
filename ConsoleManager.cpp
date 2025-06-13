#include "ConsoleManager.h"
#include <iostream>
#include "Scheduler.h"
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>

using namespace std;

ConsoleManager* ConsoleManager::consoleManager = nullptr;
int NUM_PROCESSES = 10;

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

void ConsoleManager::schedulerTest() {
    static int process_counter = 0;

    for (int i = 0; i < NUM_PROCESSES; i++) {
        process_counter++;
        string processName = "P" + std::to_string(process_counter);
        shared_ptr<Console> processConsole = make_shared<Console>(
            processName, 0, 10, ConsoleManager::getInstance()->getCurrentTimeStamp());

        ConsoleManager::getInstance()->registerConsole(processConsole);
        Scheduler::getInstance()->assignProcess(processConsole);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

unordered_map<string, shared_ptr<Console>> ConsoleManager::getScreenMap() {
    return this->screenMap;
}

void ConsoleManager::displayProcessSmi() {
    unordered_map<string, shared_ptr<Console>> screenMap = ConsoleManager::getInstance()->getScreenMap();
    Scheduler* scheduler = Scheduler::getInstance();
    int coresUsed = 0; // change (hardcoded)
    int coresAvailable = 4;
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