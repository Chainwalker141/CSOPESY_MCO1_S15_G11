#include "main.h"
#include <vector>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include "Console.h"
#include "ConsoleManager.h"
#include <cstdlib>

bool running = true;


void Exit() {
	cout << "Exiting MOOD CLI...\n";
	running = false;
}


void Initialize() {
	cout << "initialize command recognized. Doing something...\n";
	// Add initialization code here
}

void Screen(std::vector<std::string> args) {
    string processName = args[1];
    shared_ptr<Console> consoleScreen = make_shared<Console>(processName, 12, 1250, "MM/DD/YYYY, HH:MM:SS AM/PM");

    ConsoleManager::getInstance()->registerConsole(consoleScreen);
    ConsoleManager::getInstance()->drawConsole(consoleScreen->getProcessName());

    string input, command;
    cout << "Enter command: ";
    getline(cin, input); // Gets entire line
    istringstream iss(input); // Parses each string token 
    iss >> command;
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
    display_ASCII();
}

int main() {
    // Display start interface
    
    string input, command; 

    ConsoleManager::initialize();

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
        else if (command == "screen") {
            system("cls");
            Screen(args);
       /*     cout << args[1] << endl;*/
        }
        else if (command == "scheduler-test") {
            SchedulerTest();
        }
        else if (command == "scheduler-stop") { 
            SchedulerStop();
        }
        else if (command == "report-util") {
            ReportUtil();
        }
        else if (command == "clear") {
            Clear();
        }
        else {
            cout << command << " is not a recognized command. Please try again.\n";
        }

    }

    return 0;
}