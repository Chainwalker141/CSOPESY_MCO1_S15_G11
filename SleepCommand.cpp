#include "SleepCommand.h"
#include "ConsoleManager.h"
#include <thread>

SleepCommand::SleepCommand(string processName, string messageToPrint, int value, int delay)
	: ICommand(processName, ICommand::SLEEP, delay)
{
	this->messageToPrint = messageToPrint;
	this->value = value;
}

void SleepCommand::execute() {
	//TODO: Implement Print Command
	auto screenMap = ConsoleManager::getInstance()->getScreenMap();
	auto screen = screenMap.find(this->processName);

	if (screen != screenMap.end()) {
		auto console = screen->second;
		string finalMsg = this->messageToPrint;

		std::string printLog = "[" + ConsoleManager::getCurrentTimeStamp() + "] " + finalMsg + std::to_string(value) + " sec";

		// Append to console's output
		console->appendOutput(printLog);

		// Sleep for the specified duration
		std::this_thread::sleep_for(std::chrono::seconds(value));
	}
	busyWait(); // Simulate CPU cycle delay after execution
}