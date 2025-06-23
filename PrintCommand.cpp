#include "PrintCommand.h"

PrintCommand::PrintCommand(int pid, string messageToPrint) 
	: ICommand(pid, ICommand::PRINT) 
{
	this->messageToPrint = messageToPrint;
}

void PrintCommand::execute() {
	//TODO: Implement Print Command
}