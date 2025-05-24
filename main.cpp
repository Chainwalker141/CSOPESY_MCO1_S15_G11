#include "main.h"
#include <vector>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include "Console.h"
#include "ConsoleManager.h"
#include <cstdlib>

bool running = true;
bool initialized = false; 

void Exit() {
	cout << "Exiting MOOD CLI...\n";
	running = false;
}


void Initialize() {
    ConsoleManager::getInstance()->setInitialize(true); // initialize OS
}

void Screen(std::vector<std::string> args) {
    try {

        bool screenRunning = true;

        while (screenRunning) {
            if (args.size() < 2 || args.size() > 2) { // incorrect arguments checker
                throw std::runtime_error("Invalid Command Arguments \nCorrect Usage: screen -s|-r <ProcessName>");
            }

            string screenCommand = args[0]; // determines if -s or -r
            string processName = args[1];
            shared_ptr<Console> consoleScreen = make_shared<Console>(processName, 12, 1250);

            if (screenCommand == "-s") { // create a screen
                if (ConsoleManager::getInstance()->screenExists(consoleScreen->getProcessName())) {
                    cout << "screen already exists\n";
                }
                else {
                    ConsoleManager::getInstance()->registerConsole(consoleScreen);
                    cout << "screen created\n";
                    ConsoleManager::getInstance()->drawConsole(consoleScreen->getProcessName());
                }
            }
            else if (screenCommand == "-r") { // resume an existing screen
                if (ConsoleManager::getInstance()->screenExists(consoleScreen->getProcessName())) {
                    cout << "screen resumed\n";
                    ConsoleManager::getInstance()->drawConsole(consoleScreen->getProcessName());
                }
                else {
                    cout << "screen not found\n";
                }
            }
            else { // invalid command inputs
                throw std::runtime_error("Invalid Command Arguments \nCorrect Usage: screen -s|-r <ProcessName>");
            }

            string input, command;
            cout << "Enter command: ";
            getline(cin, input); // Gets entire line
            istringstream iss(input); // Parses each string token 
            iss >> command;

            if (command == "exit") { // to exit screen display
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
        cout << "An error occurred: " << e.what();
    }
    
}

void SchedulerTest() {
	cout << "scheduler-test command recognized. Doing something...\n";
	// Add scheduler test code here
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
    
    string input, command; 
    bool isCommand = false;

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
            Initialize();
        }
        else if (command == "screen" && ConsoleManager::getInstance()->getInitialize()) {
            system("cls");
            Screen(args);
       /*     cout << args[1] << endl;*/
            cout << "\n\nReturning to main menu... \n\n";
        }
        else if (command == "scheduler-test" && ConsoleManager::getInstance()->getInitialize()) {
            SchedulerTest();
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
            if (!ConsoleManager::getInstance()->getInitialize())
                cout << "Please initialized the console." << endl;
            else
                cout << command << " is not a recognized command. Please try again.\n";
        }

        command = ""; // reset command variable
    }

    return 0;
}