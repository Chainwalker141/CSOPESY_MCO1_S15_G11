#include "main.h"
#include <vector>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include "Console.h"
#include "ConsoleManager.h"
#include <cstdlib>
#include "Scheduler.h"
#include <fstream>
#include <random>
#include "FlatMemoryAllocator.h"

// Set global variables and config
bool running = true;
int NUM_CPU;
string SCHEDULER;
int QUANTUM_CYCLES;
int BATCH_PROCESS_FREQ;
int MIN_INS;
int MAX_INS;
int DELAYS_PER_EXEC;
size_t MAX_OVERALL_MEM;
size_t MEM_PER_FRAME;
size_t MIN_MEM_PER_PROC;
size_t MAX_MEM_PER_PROC;


void Exit() {
	cout << "Exiting MOOD CLI...\n";
	running = false;
}

void readConfig() {
    string filename = "config.txt";
    ifstream inFile(filename);

    if (inFile.fail()) {
        cout << "Error opening config file" << endl;
        return;
    }

    string key, value;
    while (inFile >> key >> value) {
        if (key == "num-cpu") {
            NUM_CPU = stoi(value);
        }
        else if (key == "scheduler") {
            SCHEDULER = value;
        }
        else if (key == "quantum-cycles") {
            QUANTUM_CYCLES = stoi(value);
        }
        else if (key == "batch-process-freq") {
            BATCH_PROCESS_FREQ = stoi(value);
        }
        else if (key == "min-ins") {
            MIN_INS = stoi(value);
        }
        else if (key == "max-ins") {
            MAX_INS = stoi(value);
        }
        else if (key == "delays-per-exec") {
            DELAYS_PER_EXEC = stoi(value);
        }
        else if (key == "max-overall-mem") {
            MAX_OVERALL_MEM = stoul(value);
		}
        else if (key == "mem-per-frame") {
            MEM_PER_FRAME = stoul(value);
        }
        else if (key == "min-mem-per-proc") {
            MIN_MEM_PER_PROC = stoul(value);
        }
        else if (key == "max-mem-per-proc") {
            MAX_MEM_PER_PROC = stoul(value);
        }
        else {
            cerr << "Unknown parameter: " << key << std::endl;
        }
    }
}

void printConfig() {
    cout << "CONFIG SETTINGS: " << endl;
    cout << "num_cpu: " << NUM_CPU << endl;
    cout << "scheduler: " << SCHEDULER << endl;
    cout << "quantum_cycles: " << QUANTUM_CYCLES << endl;
    cout << "batch_process_freq: " << BATCH_PROCESS_FREQ << endl;
    cout << "min_ins: " << MIN_INS << endl;
    cout << "max_ins: " << MAX_INS << endl;
    cout << "delays_per_exec: " << DELAYS_PER_EXEC << endl;
    cout << "max_overall_mem: " << MAX_OVERALL_MEM << endl;
    cout << "mem_per_frame: " << MEM_PER_FRAME << endl;
    cout << "min_mem_per_proc: " << MIN_MEM_PER_PROC << endl;
    cout << "max_mem_per_proc: " << MAX_MEM_PER_PROC << endl;
}

void Initialize() {
    readConfig();
    printConfig();
    ConsoleManager::getInstance()->setInitialize(true); // initialize OS
    ConsoleManager::getInstance()->setMaxIns(MAX_INS);
    ConsoleManager::getInstance()->setMinIns(MIN_INS);
	FlatMemoryAllocator::initialize(MAX_OVERALL_MEM, MEM_PER_FRAME); // initialize memory allocator
    Scheduler::initialize(NUM_CPU, QUANTUM_CYCLES, SCHEDULER); // initialize scheduler
    Scheduler::getInstance()->start();
    cout << "total Frames: " << FlatMemoryAllocator::getInstance()->getTotalFrames();
    cout << "\n\nMOOD OS Initialized... \n\n";
}

// helper function for checking if number is a power of 2 (for memory sizes)
bool isPowerOfTwo(size_t n) {
    if (n == 0) return false;

    while (n % 2 == 0) {
        n /= 2;
    }

    return n == 1;
}

// helper function for "screen -c" instructions 
string trim(const string& str) {
    const auto begin = str.find_first_not_of(" \t\r\n");
    const auto end = str.find_last_not_of(" \t\r\n");
    return (begin == string::npos || end == string::npos)
        ? ""
        : str.substr(begin, end - begin + 1);
}

void Screen(std::vector<std::string> args) {
    try {
        if (args.empty()) {
            throw std::runtime_error("Invalid Command Arguments \nCorrect Usage: screen -s|-r <ProcessName> OR screen -ls");
        }

        string screenCommand = args[0];

        // screen -ls
        if (screenCommand == "-ls") {
            ConsoleManager::getInstance()->displayProcessSmi();
            string input, command;
            cout << "Enter command: ";
            getline(cin, input);
            istringstream iss(input);
            iss >> command;

            if (command == "exit") {
                system("cls");
                return;
            }
            else {
                system("cls");
                cout << "Command unrecognized\n\n";
                return;
            }
        }

        // Checker for -s or -r command
        if (screenCommand == "-s") {
            if (args.size() != 3) {
                throw std::runtime_error("Invalid Command Arguments\nCorrect Usage: screen -s <ProcessName> <Memory Size>");
            }
        }
        if (screenCommand == "-r") {
            if (args.size() != 2) {
                throw std::runtime_error("Invalid Command Arguments\nCorrect Usage: screen -r <ProcessName>");
            }
        }

        string processName = args[1];

        shared_ptr<Console> consoleScreen;

        if (screenCommand == "-s") {
            size_t memorySize = stoull(args[2]);

            // check if memory size is valid
            if (!isPowerOfTwo(memorySize)) {
                throw std::runtime_error("Invalid Memory Size \nMemory size must be a power of 2");
            }

            if (ConsoleManager::getInstance()->screenExists(processName)) {
                cout << "Process " << processName << " already exists!\n";
            }
            else {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                std::uniform_int_distribution<> distrib(MIN_INS, MAX_INS);
                int totalIns = distrib(gen);

                consoleScreen = make_shared<Console>(
                    processName, 0, totalIns, ConsoleManager::getInstance()->getCurrentTimeStamp(), memorySize); // creates a process "P(N)" which has 10 lines and created at a certain time with 64 bytes of memory

                ConsoleManager::getInstance()->generateCommands(consoleScreen, DELAYS_PER_EXEC);
                ConsoleManager::getInstance()->registerConsole(consoleScreen);
                Scheduler::getInstance()->assignProcess(consoleScreen);
                cout << "screen created\n";
            }
        }
        else if (screenCommand == "-r") {
            if (ConsoleManager::getInstance()->screenExists(processName)) {
                consoleScreen = ConsoleManager::getInstance()->getScreenMap()[processName];

                if (consoleScreen->isProcessDone()) {
                    cout << "Process " << processName << " has already finished execution. Cannot resume.\n";
                    return;
                }

                cout << "screen resumed\n";
            }
            else {
                cout << "Process " << processName << " not found\n";
                return;
            }
        }
        else if (screenCommand == "-c") {
            size_t memorySize = stoull(args[2]);

            // check if memory size is valid
            if (!isPowerOfTwo(memorySize)) {
                throw std::runtime_error("Invalid Memory Size \nMemory size must be a power of 2");
            }

            if (args.size() < 4) {
                cout << "Invalid Command Arguments \nCorrect Usage: screen -c <ProcessName> <Memory Size> \"<Instructions>\"";
            }
            else if (ConsoleManager::getInstance()->screenExists(processName)) {
                cout << "Process " << processName << " already exists!\n";
            }
            else {
                std::string rawInstructionBlock;
                for (size_t i = 3; i < args.size(); ++i) {
                    if (!rawInstructionBlock.empty()) rawInstructionBlock += " ";
                    rawInstructionBlock += args[i];
                }

                // Remove outer quotes, if any
                if (!rawInstructionBlock.empty() && rawInstructionBlock.front() == '"') rawInstructionBlock.erase(0, 1);
                if (!rawInstructionBlock.empty() && rawInstructionBlock.back() == '"') rawInstructionBlock.pop_back();

                // Split by semicolon
                std::vector<std::string> instructions;
                std::stringstream ss(rawInstructionBlock);
                std::string instr;
                while (std::getline(ss, instr, ';')) {
                    instr = trim(instr);
                    if (!instr.empty()) {
                        std::cout << "[Parsed] " << instr << std::endl;  // Optional debug
                        instructions.push_back(instr);
                    }
                }

                /*debug purposes
                for (const auto& instr : instructions) {
                    std::cout << "[debug] Parsed instruction: " << instr << "\n";
                }*/

                if (instructions.size() < 1 || instructions.size() > 50) {
                    throw std::runtime_error("Invalid number of instructions (must be 1 to 50)");
                }

                // Create Console
                consoleScreen = make_shared<Console>(
                    processName, 0, instructions.size(),
                    ConsoleManager::getInstance()->getCurrentTimeStamp(), memorySize);

                // Pass the list of full instructions to ConsoleManager
                ConsoleManager::getInstance()->generateUserCommands(consoleScreen, instructions, DELAYS_PER_EXEC);
                ConsoleManager::getInstance()->registerConsole(consoleScreen);
                Scheduler::getInstance()->assignProcess(consoleScreen);

                cout << "screen created with custom instructions\n";
            }
        }
        else {
            throw std::runtime_error("Invalid Command Arguments \nCorrect Usage: screen -s|-r <ProcessName>");
        }

        // draw console once
        ConsoleManager::getInstance()->drawConsole(processName);

        // command loop within screen
        bool screenRunning = true;
        while (screenRunning) {
            string input, command;
            cout << "Enter command: ";
            getline(cin, input);
            istringstream iss(input);
            iss >> command;

            if (command == "exit") {
                system("cls");
                screenRunning = false;
            }
            else if (command == "process-smi") {
                system("cls");
                ConsoleManager::getInstance()->displayProcessSmi(processName);
                continue;
            }
            else {
                system("cls");
                cout << "Command unrecognized\n\n";
                ConsoleManager::getInstance()->drawConsole(processName);
            }
        }

    }
    catch (exception& e) {
        cout << "An error occurred: " << e.what() << endl;
    }
}


void SchedulerTest(int numCore) {
    if (Scheduler::getInstance()->getIsSchedulerTestRunning()) {
        cout << "Scheduler test is already running.\n";
		return;
    }
	cout << "Creating test processes...\n";
    
    Scheduler::getInstance()->setIsSchedulerTestRunning(true);

	// Create a separate thread that continuously generates processes based on BATCH_PROCESS_FREQ
    std::thread([]() {
        ConsoleManager::getInstance()->schedulerTest(BATCH_PROCESS_FREQ, DELAYS_PER_EXEC, MIN_MEM_PER_PROC, MAX_MEM_PER_PROC);
    }).detach();

    system("cls");
}

void SchedulerStop() {
    if (!Scheduler::getInstance()->getIsSchedulerTestRunning()) {
        cout << "\n\nNo scheduler test is currently running.\n\n";
        return;
    }
    
    cout << "Stopping scheduler test...\n";
    Scheduler::getInstance()->setIsSchedulerTestRunning(false);
	cout << "Scheduler test stopped.\n";
    system("cls");
}

void ReportUtil() {
    std::ofstream reportFile("csopesy-log.txt");
    if (!reportFile.is_open()) {
        std::cout << "Failed to open report file.\n";
    }
    else {
		Scheduler* scheduler = Scheduler::getInstance();
        auto screenMap = ConsoleManager::getInstance()->getScreenMap();
        int coresUsed = scheduler->getCoresUsed(); // TODO: change(hardcoded)
        int coresAvailable = scheduler->getCoresAvailable();
        float cpuUtilization = (float)coresUsed / (coresUsed + coresAvailable) * 100;
		reportFile << "CPU Utilization: " << cpuUtilization << "%\n";
		reportFile << "Cores Used: " << coresUsed << "\n";
		reportFile << "Cores Available: " << coresAvailable << "\n";
        reportFile << "Running processes:\n";
        for (const auto& pair : screenMap) {
            std::shared_ptr<Console> screenPtr = pair.second;
            std::string coreIDstr;
            int coreIDint = screenPtr->getCoreID();

            if (screenPtr->getCoreID() != -1) {
                coreIDstr = std::to_string(coreIDint);
            }
            else {
                coreIDstr = "N/A";
            }

            if (screenPtr->getCurrentLine() < screenPtr->getTotalLine()) {
                reportFile << "Name: " << screenPtr->getProcessName() << " | "
                    << screenPtr->getTimestamp() << " | "
                    << "Core: " << coreIDstr << " | "
                    << screenPtr->getCurrentLine() << "/"
                    << screenPtr->getTotalLine() << " | " << "\n";
            }
        }

        reportFile << "\nFinished processes:\n";
        for (const auto& pair : screenMap) {
            std::shared_ptr<Console> screenPtr = pair.second;
            if (screenPtr->getCurrentLine() >= screenPtr->getTotalLine()) {
                reportFile << "Name: " << screenPtr->getProcessName() << " | "
                    << screenPtr->getTimestamp() << " | "
                    << "Finished | "
                    << screenPtr->getCurrentLine() << "/"
                    << screenPtr->getTotalLine() << " | Finished!" << "\n";
            }
        }
        std::cout << "\n\nReport written to csopesy-log.txt\n\n";
    }
}

void Clear() {
    system("CLS");
}



int main() {
    // Display start interface
    int cpuCycles = 0;
    string input, command;
    bool isCommand = false;
    cout << "=======================================================\n" << endl;
    cout << "   Please initialize the console using \"initialize\"" << endl;
    cout << "\n=======================================================" << endl;

    ConsoleManager::initialize(); // initializing console manager instance

    while (running) {
        display_ASCII();
        cout << "Enter command: ";
        getline(cin, input); // Gets entire line
        istringstream iss(input); // Parses each string token 
        iss >> command;

        std::vector<std::string> args;
        std::string word;

        while (iss >> word) {
            args.push_back(word); // gets "north" and "fast"
        }

        if (command == "exit") {
            Exit();
        }
        else if (command == "initialize") {
            if (!ConsoleManager::getInstance()->getInitialize()) {
                system("cls");
                Initialize();
            }
            else {
                system("cls");
                cout << "Console is already initialized!\n";
            }
            
        }
        else if (command == "screen" && ConsoleManager::getInstance()->getInitialize()) {
            system("cls");
            Screen(args);
       /*     cout << args[1] << endl;*/
            cout << "\n\nReturning to main menu... \n\n";
        }
        else if (command == "scheduler-start" && ConsoleManager::getInstance()->getInitialize()) {
            system("cls");
            SchedulerTest(NUM_CPU);
            cout << "\n\nTest Processes created, please type \"screen -ls\" to view... \n\n";
        }
        else if (command == "scheduler-stop" && ConsoleManager::getInstance()->getInitialize()) {
            SchedulerStop();
            cout << "\n\nHalting dummy process creation... \n\n";
        }
        else if (command == "report-util" && ConsoleManager::getInstance()->getInitialize()) {
            system("cls");
            ReportUtil();
        }
        else if (command == "clear" && ConsoleManager::getInstance()->getInitialize()) {
            Clear();
        }
        else {
            system("cls");
            if (!ConsoleManager::getInstance()->getInitialize()) {
                cout << "=======================================================\n" << endl;
                cout << "   Please initialize the console using \"initialize\"" << endl;
                cout << "\n=======================================================" << endl;
            }
            else
                cout << command << " is not a recognized command. Please try again.\n";
        }

        command = ""; // reset command variable
        cpuCycles++;
    }

    return 0;
}