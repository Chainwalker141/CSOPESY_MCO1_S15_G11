#pragma once
#include "ICommand.h"
#include <string>

using namespace std;

class PrintCommand : public ICommand {
public:
	PrintCommand(int pid, string messageToPrint);
	void execute() override;

private:
	string messageToPrint;
};
