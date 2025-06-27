#include "PrintCommand.h"
#include "ConsoleManager.h"
#include <iostream>

PrintCommand::PrintCommand(string processName, string messageToPrint, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable,int delay)
	: ICommand(processName, ICommand::PRINT, delay)
{
	this->messageToPrint = messageToPrint;
	this->varTable = varTable;
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

		std::string printLog = "[" + ConsoleManager::getCurrentTimeStamp() + "] " + finalMsg;

		// Append to console's output
		console->appendOutput(printLog);
	}
	busyWait(); // Simulate CPU cycle delay after execution
}