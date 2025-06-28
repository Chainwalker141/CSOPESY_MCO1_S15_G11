#include "SleepCommand.h"
#include "ConsoleManager.h"

SleepCommand::SleepCommand(string processName, string messageToPrint, int value, int delay)
	: ICommand(processName, ICommand::SLEEP, delay)
{
	this->messageToPrint = messageToPrint;
}

void SleepCommand::execute() {
	//TODO: Implement Print Command
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