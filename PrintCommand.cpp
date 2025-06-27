#include "PrintCommand.h"
#include "ConsoleManager.h"
#include <iostream>

PrintCommand::PrintCommand(string processName, string messageToPrint, int delay)
	: ICommand(processName, ICommand::PRINT, delay)
{
	this->messageToPrint = messageToPrint;
}

void PrintCommand::execute() {
	// Get the target console for this process
	auto screenMap = ConsoleManager::getInstance()->getScreenMap();
	auto screen = screenMap.find(this->processName);

	if (screen != screenMap.end()) {
		auto console = screen->second;
		auto varTable = console->getVarTable();
		string finalMsg;

		// find if it is a variable name
		auto it = varTable->find(this->messageToPrint);
		if (it != varTable->end()) {
			finalMsg = "Value from " + it->first + ": " + std::to_string(it->second);
		}
		// otherwise default
		else {
			finalMsg = "Hello world from " + this->processName;
		}

		// append to console's output
		console->appendOutput(finalMsg);
	}
	busyWait(); // Simulate CPU cycle delay after execution
}