#include "main.h"
#include <vector>
#include <sstream>
#include <iostream>

bool running = true;


void Exit() {
	cout << "Exiting MOOD CLI...\n";
	running = false;
}


void Initialize() {
	cout << "Initialize command recognized. Doing something...\n";
	// Add initialization code here
}

void Screen() {
	cout << "Screen command recognized. Doing something...\n";
	// Add screen code here
}

void SchedulerTest() {
	cout << "Scheduler test command recognized. Doing something...\n";
	// Add scheduler test code here
}

void SchedulerStop() {
	cout << "Scheduler stop command recognized. Doing something...\n";
	// Add scheduler stop code here
}

void ReportUtil() {
	cout << "Report utilization command recognized. Doing something...\n";
	// Add report utilization code here
}

void Clear() {
    system("CLS");
    display_ASCII();
}

int main() {
    // Display start interface
    display_ASCII();
    string input, command; 

    while (running) {
        cout << "Enter command: ";
        getline(cin, input); // Gets entire line
        istringstream iss(input); // Parses each string token 
        iss >> command; 

        if (command == "exit") {
            Exit();
        }
        else if (command == "initialize") {
            Initialize();
        }
        else if (command == "screen") {
            Screen();
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
            cout << command << " is not a unrecognized command. Please try again.\n";
        }

    }

    return 0;
}