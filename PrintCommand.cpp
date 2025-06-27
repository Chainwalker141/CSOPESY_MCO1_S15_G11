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
	auto it = screenMap.find(this->processName);

	if (it != screenMap.end()) {
		auto console = it->second;

		// Append output to console's internal buffer
		console->appendOutput(this->messageToPrint);
	}
	busyWait(); // Simulate CPU cycle delay after execution
}