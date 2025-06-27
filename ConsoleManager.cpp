#include "ConsoleManager.h"
#include <iostream>
#include "Scheduler.h"
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <random>
// types of commands
#include "PrintCommand.h"
#include "DeclareCommand.h"
#include "AddCommand.h"
#include "SubCommand.h"
#include "SleepCommand.h"
#include "ForCommand.h"

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

void ConsoleManager::schedulerTest(int NUM_PROCESSES, int DELAYS_PER_EXEC) {
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

            ConsoleManager::getInstance()->generateCommands(processConsole, DELAYS_PER_EXEC);
            ConsoleManager::getInstance()->registerConsole(processConsole);
            Scheduler::getInstance()->assignProcess(processConsole);

            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // delay in process creation (ALSO IS CPU TICK IG??)
        }

    }
    
}

int ConsoleManager::generateRandInt(int minIns, int maxIns) {
    static std::random_device rd;   
    static std::mt19937 gen(rd());    
    std::uniform_int_distribution<> distrib(minIns, maxIns);
    return distrib(gen);
}

void ConsoleManager::generateCommands(std::shared_ptr<Console> process, int DELAYS_PER_EXEC) {
    std::queue<std::shared_ptr<ICommand>> commandList;
    int totalIns = process->getTotalLine();

    string processName = process->getProcessName();
    string msg = "Hello world from " + processName + "!"; // msg to be printed DELETE SOON

    auto varTable = process->getVarTable();

    // RANDOM INSTRUCTIONS
    default_random_engine generator(static_cast<unsigned>(time(nullptr)));
    uniform_int_distribution<int> commandDist(0, 0); // 0 = Declare, 1 = Add, 2 = Sub, 3 = Print and so on
    uniform_int_distribution<int> modeDist(0, 100); // for determining which mode of the instruction to use
    uniform_int_distribution<int> valueDist(1, 500); // random values for Declare, Add, Sub
    int generatedVars = 0;

    for (int i = 0; i < totalIns; i++) {
        int commandType = commandDist(generator);

        switch (commandType) {
        case 0: { // PRINT
            int mode = modeDist(generator) % 2;
            switch (mode) {
            case 0: { // no var
                commandList.push(std::make_shared<PrintCommand>(processName, "Hello World! (this wont actually print tho lol)", varTable, DELAYS_PER_EXEC));
                break;
            }
            case 1: { // with var
                int value = valueDist(generator);
                string varName = "var" + to_string(generatedVars++);
                commandList.push(std::make_shared<DeclareCommand>(processName, varName, value, varTable, DELAYS_PER_EXEC));
                commandList.push(std::make_shared<PrintCommand>(processName, varName, varTable, DELAYS_PER_EXEC));
                break;
            }
            }
            
            break;
        }
        case 1: { // DECLARE
            int value = valueDist(generator);
            string varName = "var" + to_string(generatedVars++);
            commandList.push(std::make_shared<DeclareCommand>(processName, varName, value, varTable, DELAYS_PER_EXEC));
            break;
        }
        case 2: { // ADD
            int value = valueDist(generator);
            string varName = "var" + to_string(generatedVars++);
            //commandList.push(std::make_shared<AddCommand>(processName, "sum", value, "val", varTable, DELAYS_PER_EXEC));
            break;
        }
        case 3: { // SUBTRACT
            int value = valueDist(generator);
            string varName = "var" + to_string(generatedVars++);
            //commandList.push(std::make_shared<SubCommand>(processName, "diff", "val", value, varTable, DELAYS_PER_EXEC));
            break;
        }
        case 4: { // SLEEP
            //commandList.push(std::make_shared<PrintCommand>(processName, "val", DELAYS_PER_EXEC));
            break;
        }
        case 5: { // FOR
            //commandList.push(std::make_shared<PrintCommand>(processName, "val", DELAYS_PER_EXEC));
            break;
        }
        }
    }
    process->setCommandList(commandList);
}

unordered_map<string, shared_ptr<Console>> ConsoleManager::getScreenMap() {
    return this->screenMap;
}

// screen -ls
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

    // Running Processes
    cout << "Running processes:" << endl;
    for (const auto& pair : screenMap) {
        shared_ptr<Console> screenPtr = pair.second;

        auto coreID = screenPtr->getProcessName();
        string coreIDstr;
        if (screenPtr->getCurrentLine() < screenPtr->getTotalLine()) {
            cout << "Name: " << screenPtr->getProcessName() << " | "
                << screenPtr->getTimestamp() << " | "
                << "Core: " << coreIDstr << " | "
                << screenPtr->getCurrentLine() << "/"
                << screenPtr->getTotalLine() << " | " << endl;
        }
        /*   if (coreID == -1) {
               coreIDstr = "N/A";
           }
           else {
               coreIDstr = to_string(coreID);
           }*/
        
    }

    // Finished Processes
    cout << "\nFinished Processes:" << endl;
    for (const auto& pair : screenMap) {
        auto screenPtr = pair.second;
        if (screenPtr->getCurrentLine() >= screenPtr->getTotalLine()) {
            cout << "Name: " << screenPtr->getProcessName() << " | "
                << screenPtr->getTimestamp() << " | "
                << "Finished | "
                <<screenPtr->getCurrentLine() << "/"
                << screenPtr->getTotalLine() << " | Finished!" << endl;
        }
    }
}

//screen -r >> process-smi
void ConsoleManager::displayProcessSmi(const std::string& processName) {
    auto it = screenMap.find(processName);
    if (it == screenMap.end()) {
        std::cout << "Process " << processName << " not found." << std::endl;
        return;
    }

    shared_ptr<Console> screenPtr = it->second;

    std::cout << "Name: " << screenPtr->getProcessName() << std::endl;
    std::cout << "Progress: " << screenPtr->getCurrentLine() << "/" << screenPtr->getTotalLine() << std::endl;
    std::cout << "Timestamp: " << screenPtr->getTimestamp() << std::endl;

    std::cout << "Logs:" << std::endl;
    for (const auto& log : screenPtr->getOutputBuffer()) {
        std::cout << "  " << log << std::endl;
    }

    if (screenPtr->getCurrentLine() >= screenPtr->getTotalLine()) {
        std::cout << "Finished!" << std::endl;
    }
}