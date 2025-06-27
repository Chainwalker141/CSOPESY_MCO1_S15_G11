#include "SleepCommand.h"

SleepCommand::SleepCommand(string processName, string messageToPrint, int delay)
	: ICommand(processName, ICommand::SLEEP, delay)
{
	this->messageToPrint = messageToPrint;
}

void SleepCommand::execute() {
	//TODO: Implement Print Command

	busyWait(); // Simulate CPU cycle delay after execution
}