#pragma once


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
		
	ICommand(int pid, ICommand::CommandType commandType);
	CommandType getCommandType();
	virtual void execute() = 0;
protected: 
	int pid;
	CommandType commandType;
};

inline ICommand::CommandType ICommand::getCommandType() {
	return this->commandType;
}

inline ICommand::ICommand(int pid, ICommand::CommandType commandType) {
	this->pid = pid;
	this->commandType = commandType;
}