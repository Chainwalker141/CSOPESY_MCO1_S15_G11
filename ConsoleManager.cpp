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
std::mutex readWriteSpaceMutex;

ConsoleManager::ConsoleManager() {
};

void ConsoleManager::initialize() {
    consoleManager = new ConsoleManager();
    readWriteSpace = std::make_shared<std::unordered_map<string, AddressBlock>>();
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

size_t ConsoleManager::generateRandBase2(size_t minVal, size_t maxVal) {
    // Clamp input range to [2^6, 2^16]
    const size_t minClamp = 1ULL << 6;   // 64
    const size_t maxClamp = 1ULL << 16;  // 65536

    // Adjust minVal and maxVal to stay within the allowed range
    minVal = std::max(minVal, minClamp);
    maxVal = std::min(maxVal, maxClamp);

    // Handle edge case where minVal > maxVal after clamping
    if (minVal > maxVal)
        return minClamp;

    int minExp = static_cast<int>(std::ceil(std::log2(minVal)));
    int maxExp = static_cast<int>(std::floor(std::log2(maxVal)));

    if (minExp > maxExp)
        return 1ULL << minExp;

    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(minExp, maxExp);

    return 1ULL << dist(gen);
}

void ConsoleManager::schedulerTest(int NUM_PROCESSES, int DELAYS_PER_EXEC, size_t MIN_MEM_PER_PROC, size_t MAX_MEM_PER_PROC, size_t MEM_PER_FRAME) {
    static int process_counter = 0;
    while (Scheduler::getInstance()->getIsSchedulerTestRunning()) {
        for (int i = 0; i < NUM_PROCESSES; i++) {
            //cout << "Creating Process " << process_counter << endl;
            process_counter++;
            string processName = "P" + std::to_string(process_counter);

            //int totalIns = 100;
            int totalIns = generateRandInt(minIns, maxIns); // Generate random instruction total
            size_t memSize = generateRandBase2(MIN_MEM_PER_PROC, MAX_MEM_PER_PROC);
            
            shared_ptr<Console> processConsole = make_shared<Console>(
                processName, 0, totalIns, ConsoleManager::getInstance()->getCurrentTimeStamp(), memSize); // creates a process "P(N)" which has 10 lines and created at a certain time with a random memSize
            processConsole->initializePageTable(MEM_PER_FRAME);

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

// helper command, trim strings
inline std::string cmTrim(const std::string& str) {
    const char* whitespace = " \t\n\r\f\v";
    size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos) return ""; // All whitespace

    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

void ConsoleManager::generateCommands(std::shared_ptr<Console> process, int DELAYS_PER_EXEC) {
    std::queue<std::shared_ptr<ICommand>> commandList;
    int totalIns = process->getTotalLine();

    string processName = process->getProcessName();

    // ok so, for each instruction it has to search for the screen again just so it can access it's output buffer function for logs
    // its REALLY stupid and should be fixed when possible like send the function itself or smthing over here
    // TODO: send output buffer through here as a parameter

    auto varTable = process->getVarTable();

    // RANDOM INSTRUCTIONS
    default_random_engine generator(static_cast<unsigned>(time(nullptr)));
    uniform_int_distribution<int> commandDist(0, 3); // 0 = Declare, 1 = Add, 2 = Sub, 3 = Print and so on
    uniform_int_distribution<int> modeDist(0, 100); // for determining which mode of the instruction to use
    uniform_int_distribution<int> valueDist(1, 10); // random values for Declare, Add, Sub
	uniform_int_distribution<int> sleepDist(1, 5); // random sleep time for Sleep command
    int generatedVars = 0;

    for (int i = 0; i < totalIns; i++) {
        int commandType = 3;

        

        switch (commandType) {
        case 0: { // PRINT
            int mode = 1;
            
            switch (mode) {
            case 0: { // no var
                commandList.push(std::make_shared<PrintCommand>(processName, "Hello World! (this wont actually print tho lol)", varTable, DELAYS_PER_EXEC));
                break;
            }
            case 1: { // with var
                int value = valueDist(generator);
                string varName = "x"; //+ to_string(generatedVars++);
                //commandList.push(std::make_shared<DeclareCommand>(processName, varName, value, varTable, DELAYS_PER_EXEC));
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
            int val1 = valueDist(generator); // val 1
            int val2 = valueDist(generator); // val 2
            int mode = 1;
            string sumVarName = "x";//"sum" + to_string(generatedVars++); // var to store number

            switch (mode) {
            case 0: { // both direct number
                commandList.push(std::make_shared<AddCommand>(processName, sumVarName, val1, val2, varTable, DELAYS_PER_EXEC));
                break;
            }
            case 1: { // var 1 and number
                string varName1 = "x"; //+ to_string(generatedVars++);
                //commandList.push(std::make_shared<DeclareCommand>(processName, varName1, val1, varTable, DELAYS_PER_EXEC)); // declare a var first
                commandList.push(std::make_shared<AddCommand>(processName, sumVarName, varName1, val2, varTable, DELAYS_PER_EXEC)); // add
                break;
            }
            case 2: { // number and var 2
                string varName2 = "var" + to_string(generatedVars++);
                commandList.push(std::make_shared<DeclareCommand>(processName, varName2, val2, varTable, DELAYS_PER_EXEC)); // declare a var first
                commandList.push(std::make_shared<AddCommand>(processName, sumVarName, val1, varName2, varTable, DELAYS_PER_EXEC));// add
                break;
            }
            case 3: { // var 1 and var 2
                string varName1 = "var" + to_string(generatedVars++);
                string varName2 = "var" + to_string(generatedVars++);
                commandList.push(std::make_shared<DeclareCommand>(processName, varName1, val1, varTable, DELAYS_PER_EXEC)); // declare var1
                commandList.push(std::make_shared<DeclareCommand>(processName, varName2, val2, varTable, DELAYS_PER_EXEC)); // declare var2
                commandList.push(std::make_shared<AddCommand>(processName, sumVarName, varName1, varName2, varTable, DELAYS_PER_EXEC)); // add var1 and var2
                break;
            }
            }
            //commandList.push(std::make_shared<AddCommand>(processName, "sum", value, "val", varTable, DELAYS_PER_EXEC));
            break;
        }
        case 3: { // SUBTRACT
            int val1 = valueDist(generator); // val 1
            int val2 = valueDist(generator); // val 2
            int mode = modeDist(generator) % 4;
            string diffVarName = "diff" + to_string(generatedVars++); // var to store number

            switch (mode) {
            case 0: { // both direct number
                commandList.push(std::make_shared<SubCommand>(processName, diffVarName, val1, val2, varTable, DELAYS_PER_EXEC));
                break;
            }
            case 1: { // var 1 and number
                string varName1 = "var" + to_string(generatedVars++);
                commandList.push(std::make_shared<DeclareCommand>(processName, varName1, val1, varTable, DELAYS_PER_EXEC)); // declare a var first
                commandList.push(std::make_shared<SubCommand>(processName, diffVarName, varName1, val2, varTable, DELAYS_PER_EXEC)); // sub
                break;
            }
            case 2: { // number and var 2
                string varName2 = "var" + to_string(generatedVars++);
                commandList.push(std::make_shared<DeclareCommand>(processName, varName2, val2, varTable, DELAYS_PER_EXEC)); // declare a var first
                commandList.push(std::make_shared<SubCommand>(processName, diffVarName, val1, varName2, varTable, DELAYS_PER_EXEC));// sub
                break;
            }
            case 3: { // var 1 and var 2
                string varName1 = "var" + to_string(generatedVars++);
                string varName2 = "var" + to_string(generatedVars++);
                commandList.push(std::make_shared<DeclareCommand>(processName, varName1, val1, varTable, DELAYS_PER_EXEC)); // declare var1
                commandList.push(std::make_shared<DeclareCommand>(processName, varName2, val2, varTable, DELAYS_PER_EXEC)); // declare var2
                commandList.push(std::make_shared<SubCommand>(processName, diffVarName, varName1, varName2, varTable, DELAYS_PER_EXEC)); // sub var1 and var2
                break;
            }
            }
            //commandList.push(std::make_shared<SubCommand>(processName, "sum", value, "val", varTable, DELAYS_PER_EXEC));
            break;
        }
        case 4: { // SLEEP
            //commandList.push(std::make_shared<PrintCommand>(processName, "val", DELAYS_PER_EXEC));
			int value = sleepDist(generator);
			commandList.push(std::make_shared<SleepCommand>(processName, "Sleeping for ", value, DELAYS_PER_EXEC));
            break;
        }
        case 5: { // FOR
            int loopStart = 0; // e.g., 0-4
            int loopEnd = 2; // e.g., 1-5 steps

            auto forCmd = std::make_shared<ForCommand>(processName, "forLoop", loopStart, loopEnd, DELAYS_PER_EXEC);
			forCmd->addCommand(std::make_shared<AddCommand>(processName, "var3", 2, 2, varTable, DELAYS_PER_EXEC));

            commandList.push(forCmd);
            break;
        }
        }
    }
    process->setCommandList(commandList);
}

// helper function for generateUserCommands
std::shared_ptr<ICommand> ConsoleManager::parseInstruction(const std::string& line, std::shared_ptr<Console> process,int delays) {
    std::istringstream iss(line);
    std::string keyword;
    iss >> keyword;

    auto varTable = process->getVarTable();
    std::string processName = process->getProcessName();

    if (keyword == "DECLARE") {
        std::string varName;
        int value;
        iss >> varName >> value;

        int currentVarCount = static_cast<int>(varTable->size());
        if (currentVarCount >= 32) {
            throw std::runtime_error("Variable limit exceeded: Only 32 variables allowed.");
        }

        if (value < 0 || value > 65535) {
            throw std::runtime_error("Variable value out of range: Must be between 0 and 65535 (uint16_t).");
        }

        return std::make_shared<DeclareCommand>(processName, varName, value, varTable, delays);
    }

    else if (keyword == "PRINT") {
        std::string remainder;
        std::getline(iss, remainder);
        remainder = cmTrim(remainder);

        if (remainder.front() == '(' && remainder.back() == ')') {
            remainder = remainder.substr(1, remainder.size() - 2); // remove parentheses
        }

        // split on '+' and trim each
        std::vector<std::string> parts;
        std::stringstream ss(remainder);
        std::string segment;
        while (std::getline(ss, segment, '+')) {
            parts.push_back(cmTrim(segment));
        }

        return std::make_shared<PrintCommand>(process->getProcessName(), parts, process->getVarTable(), delays);
    }

    else if (keyword == "ADD") {
        std::string dest, op1, op2;
        iss >> dest >> op1 >> op2;

        // try to parse both operands as integers
        bool isOp1Num = std::isdigit(op1[0]) || (op1[0] == '-' && std::isdigit(op1[1]));
        bool isOp2Num = std::isdigit(op2[0]) || (op2[0] == '-' && std::isdigit(op2[1]));

        if (isOp1Num && isOp2Num) {
            return std::make_shared<AddCommand>(processName, dest, std::stoi(op1), std::stoi(op2), varTable, delays);
        }
        else if (!isOp1Num && isOp2Num) {
            return std::make_shared<AddCommand>(processName, dest, op1, std::stoi(op2), varTable, delays);
        }
        else if (isOp1Num && !isOp2Num) {
            return std::make_shared<AddCommand>(processName, dest, std::stoi(op1), op2, varTable, delays);
        }
        else {
            return std::make_shared<AddCommand>(processName, dest, op1, op2, varTable, delays);
        }
    }

    else if (keyword == "SUBTRACT") {
        std::string dest, op1, op2;
        iss >> dest >> op1 >> op2;

        bool isOp1Num = std::isdigit(op1[0]) || (op1[0] == '-' && std::isdigit(op1[1]));
        bool isOp2Num = std::isdigit(op2[0]) || (op2[0] == '-' && std::isdigit(op2[1]));

        if (isOp1Num && isOp2Num) {
            return std::make_shared<SubCommand>(processName, dest, std::stoi(op1), std::stoi(op2), varTable, delays);
        }
        else if (!isOp1Num && isOp2Num) {
            return std::make_shared<SubCommand>(processName, dest, op1, std::stoi(op2), varTable, delays);
        }
        else if (isOp1Num && !isOp2Num) {
            return std::make_shared<SubCommand>(processName, dest, std::stoi(op1), op2, varTable, delays);
        }
        else {
            return std::make_shared<SubCommand>(processName, dest, op1, op2, varTable, delays);
        }
    }

    else if (keyword == "SLEEP") {
        int time;
        iss >> time;
        return std::make_shared<SleepCommand>(processName, "Sleeping for ", time, delays);
    }

    else if (keyword == "FOR") {
        int start, end;
        iss >> start >> end;

        auto forCmd = std::make_shared<ForCommand>(processName, "forLoop", start, end, delays);
        forCmd->addCommand(std::make_shared<AddCommand>(processName, "var3", 2, 2, varTable, delays));
        return forCmd;
    }

    else {
        throw std::runtime_error("Unknown command: " + keyword);
    }
}

// for screen -c
void ConsoleManager::generateUserCommands(std::shared_ptr<Console> process, const std::vector<std::string>& instructions, int DELAYS_PER_EXEC) {
    std::queue<std::shared_ptr<ICommand>> commandList;
    auto varTable = process->getVarTable();
    std::string processName = process->getProcessName();

    int memSize = process->getMemSize();
    int maxInstructions = (memSize - 64) / 2;
    if (instructions.size() > maxInstructions) {
        throw std::runtime_error(
            "Invalid Command.\nInstruction count exceeds memory limit. Max allowed: " + std::to_string(maxInstructions)
        );
    }

    for (const auto& line : instructions) {
        try {
            std::shared_ptr<ICommand> cmd = parseInstruction(line, process, DELAYS_PER_EXEC);
            commandList.push(cmd);
            std::cout << "[Parsed] " << line << std::endl; // debug
        }
        catch (const std::exception& e) {
            std::cerr << "[screen -c] Failed to parse: \"" << line << "\" - " << e.what() << std::endl;
            std::cerr << "Invalid Command\n";
        }
    }

    if (commandList.empty()) {
        return;  // Don't set command list or proceed
    }

    process->setCommandList(commandList);
}

unordered_map<string, shared_ptr<Console>> ConsoleManager::getScreenMap() {
    return this->screenMap;
}

void ConsoleManager::writeToAddress(string address, string processName, uint16_t value)
{
    std::lock_guard<std::mutex> lock(readWriteSpaceMutex);

    // Ensure readWriteSpace is initialized
    if (!readWriteSpace) {
        readWriteSpace = std::make_shared<std::unordered_map<string, AddressBlock>>();
    }

    std::string key = address;

    auto it = readWriteSpace->find(key);
    if (it != readWriteSpace->end() && it->second.processName != processName) { // Accessing address from a different process
        // Memory Access Error
        cout << "Process " + processName + "accessed memory out of its scope.";
        return;
    }

    // Write value to the AddressBlock (assuming AddressBlock has a suitable interface)
    (*readWriteSpace)[key].value = value;
    (*readWriteSpace)[key].processName = processName;
}

uint16_t ConsoleManager::readAddress(string address, string processName)
{
    std::lock_guard<std::mutex> lock(readWriteSpaceMutex);

    // Ensure readWriteSpace is initialized
    if (!readWriteSpace) {
        readWriteSpace = std::make_shared<std::unordered_map<string, AddressBlock>>();
    }

    std::string key = address;

    auto it = readWriteSpace->find(key);

    if (it != readWriteSpace->end()) {
        return 0;
    }
     
    if (it->second.processName != processName) {
        cout << "Process " + processName + "accessed memory out of its scope.";
        // Memory Access error 
        // Kill program
        return 999;
    }

    return it->second.value;
    // Return 0 or handle as needed if not found or processName mismatch
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
		
        std::string coreIDstr;


        int coreIDint = screenPtr->getCoreID();

        if (screenPtr->getCoreID() != -1) {
            coreIDstr = std::to_string(coreIDint);
        }
        else {
            coreIDstr = "N/A";
        }

        if (screenPtr->getCurrentLine() < screenPtr->getTotalLine()) {
            cout << "Name: " << screenPtr->getProcessName() << " | "
                << screenPtr->getTimestamp() << " | "
                << "Core: " << coreIDstr << " | "
                << screenPtr->getCurrentLine() << "/"
                << screenPtr->getTotalLine() << " | " << endl;
        }
        
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
