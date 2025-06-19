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

// Set global variables and config
bool running = true;
int NUM_CPU;
string SCHEDULER;
int QUANTUM_CYCLES;
int BATCH_PROCESS_FREQ;
int MIN_INS;
int MAX_INS;
int DELAYS_PER_EXEC;


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
}

void Initialize() {
    
    readConfig();
    printConfig();
    ConsoleManager::getInstance()->setInitialize(true); // initialize OS
    cout << "\n\nMOOD OS Initialized... \n\n";
}

void Screen(std::vector<std::string> args) {
    try {
        bool screenRunning = true;

        while (screenRunning) {
            if (args.empty()) {
                throw std::runtime_error("Invalid Command Arguments \nCorrect Usage: screen -s|-r <ProcessName> OR screen -ls");
            }

            string screenCommand = args[0];

            if (screenCommand == "-ls") {
                ConsoleManager::getInstance()->displayProcessSmi();

                string input, command;
                cout << "Enter command: ";
                getline(cin, input);
                istringstream iss(input);
                iss >> command;

                if (command == "exit") {
                    system("cls");
                    screenRunning = false;
                }
                else {
                    system("cls");
                    cout << "Command unrecognized\n\n";
                }

                return; // Exit function after -ls
            }

            // From here on: only for -s or -r with <ProcessName>
            if (args.size() != 2) {
                throw std::runtime_error("Invalid Command Arguments \nCorrect Usage: screen -s|-r <ProcessName>");
            }

            string processName = args[1];
            shared_ptr<Console> consoleScreen = make_shared<Console>(processName, 12, 1250, "MM/DD/YYYY, HH:MM:SS AM/PM"); // TODO: ADD ACTUAL DATE & TIME

            if (screenCommand == "-s") {
                if (ConsoleManager::getInstance()->screenExists(consoleScreen->getProcessName())) {
                    cout << "Process " << processName << " already exists!\n";
                }
                else {
                    ConsoleManager::getInstance()->registerConsole(consoleScreen);
                    cout << "screen created\n";
                    ConsoleManager::getInstance()->drawConsole(consoleScreen->getProcessName());
                }
            }
            else if (screenCommand == "-r") {
                if (ConsoleManager::getInstance()->screenExists(consoleScreen->getProcessName())) {
                    cout << "screen resumed\n";
                    ConsoleManager::getInstance()->drawConsole(consoleScreen->getProcessName());
                }
                else {
                    cout << "Process " << processName << " not found\n";
                }
            }
            else {
                throw std::runtime_error("Invalid Command Arguments \nCorrect Usage: screen -s|-r <ProcessName>");
            }

            string input, command;
            cout << "Enter command: ";
            getline(cin, input);
            istringstream iss(input);
            iss >> command;

            if (command == "exit") {
                system("cls");
                screenRunning = false;
            }
            else {
                system("cls");
                cout << "Command unrecognized\n\n";
            }
        }
    }
    catch (exception& e) {
        cout << "An error occurred: " << e.what() << endl;
    }
}


void SchedulerTest(int numCore) {
	cout << "Creating dummy processes. Please wait...\n";
	// Add scheduler test code here
    Scheduler::initialize(numCore, QUANTUM_CYCLES, SCHEDULER);
    ConsoleManager::getInstance()->schedulerTest(BATCH_PROCESS_FREQ);
    Scheduler::getInstance()->start();
    system("cls");
}

void SchedulerStop() {
	cout << "scheduler-stop command recognized. Doing something...\n";
	// Add scheduler stop code here
}

void ReportUtil() {
	cout << "report-util command recognized. Doing something...\n";
	// Add report utilization code here
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
        else if (command == "scheduler-test" && ConsoleManager::getInstance()->getInitialize()) {
            system("cls");
            SchedulerTest(NUM_CPU);
            cout << "\n\nTest Processes created, please type \"screen -ls\" to view... \n\n";
        }
        else if (command == "scheduler-stop" && ConsoleManager::getInstance()->getInitialize()) {
            SchedulerStop();
        }
        else if (command == "report-util" && ConsoleManager::getInstance()->getInitialize()) {
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