#include "DeclareCommand.h"


using namespace std;

DeclareCommand::DeclareCommand(int pid, string varName, uint16_t value, std::shared_ptr<std::unordered_map < string, uint16_t>> varTable)
	: ICommand(pid, ICommand::DECLARE) 
{
	this->varName = varName;
	this->value = value;
	this->varTable = varTable;
}

void DeclareCommand::execute() {

	auto keyVal = varTable->find(varName);

	// Check existence in table
	if (keyVal != varTable->end()) { // Exists: replace
		keyVal->second = value;
	}
	else {
		varTable->insert({ varName, value }); // DNE: Add
	}
}