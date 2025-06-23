#include "DeclareCommand.h"
#include <iostream> // TESTING


using namespace std;

DeclareCommand::DeclareCommand(string processName, string varName, uint16_t value, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable)
	: ICommand(processName, ICommand::DECLARE) 
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

	//cout << this->processName << " " << "declared " << varTable->find(varName)->first << " = " << varTable->find(varName)->second << "\n"; // COMMENT OUT. FOR TESTING
}