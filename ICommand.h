#pragma once
#include <string>

using namespace std;

// Interface and base class for process commands
class ICommand {
public: 
	virtual ~ICommand() = default; // Destructor function, Default cleanup 

	enum CommandType {
		PRINT, 
		DECLARE, 
		ADD, 
		SUBTRACT, 
		SLEEP, 
		FOR,
	};
		
	ICommand(string processName, ICommand::CommandType commandType);
	CommandType getCommandType();
	virtual void execute() = 0;
protected: 
	string processName;
	CommandType commandType;
};

inline ICommand::CommandType ICommand::getCommandType() {
	return this->commandType;
}

inline ICommand::ICommand(string processName, ICommand::CommandType commandType) {
	this->processName = processName;
	this->commandType = commandType;
}