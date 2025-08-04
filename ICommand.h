#pragma once
#include <string>
#include <chrono>
#include <iostream> // TODO: might need removal later?

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
		WRITE, 
		READ
	};
		
	ICommand(string processName, ICommand::CommandType commandType, int delay);
	CommandType getCommandType();
	virtual void execute() = 0;
protected: 
	string processName;
	CommandType commandType;
	int delay;

	void busyWait();
};

inline ICommand::CommandType ICommand::getCommandType() {
	return this->commandType;
}

inline ICommand::ICommand(string processName, ICommand::CommandType commandType, int delay) {
	this->processName = processName;
	this->commandType = commandType;
	this->delay = delay;
}

// simulates a busyWait where the next instruction is delayed by a set time in seconds
inline void ICommand::busyWait() {
	auto start = chrono::high_resolution_clock::now();
	while (true) {
		auto now = chrono::high_resolution_clock::now();
		auto elapsed = chrono::duration_cast<chrono::seconds>(now - start);
		if (elapsed.count() >= delay) {
			break;
		}
	}
}