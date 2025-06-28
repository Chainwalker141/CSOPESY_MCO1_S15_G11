#pragma once
#include "ICommand.h"
#include <string>

using namespace std;

class SleepCommand : public ICommand {
public:
	SleepCommand(string processName, string messageToPrint, int value, int delay);
	void execute() override;

private:
	string messageToPrint;
	int value; // Sleep duration in seconds
};
