#include "DeclareCommand.h"
#include "ConsoleManager.h"
#include <iostream> // TESTING


using namespace std;

DeclareCommand::DeclareCommand(string processName, string varName, uint16_t value, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay)
	: ICommand(processName, ICommand::DECLARE, delay) 
{
	this->varName = varName;
	this->value = value;
	this->varTable = varTable;
}

void DeclareCommand::execute() {
	auto screenMap = ConsoleManager::getInstance()->getScreenMap();
	auto screen = screenMap.find(this->processName);

	if (screen != screenMap.end()) {
		auto console = screen->second;
		auto keyVal = varTable->find(varName);

		// Check existence in table
		if (keyVal != varTable->end()) { // Exists: replace
			keyVal->second = value;
		}
		else {
			varTable->insert({ varName, value }); // DNE: Add
		}

		std::string printLog = "[" + ConsoleManager::getCurrentTimeStamp() + "] " +
			"Created Variable: " + varName + " = " + std::to_string(value);

		console->appendOutput(printLog);
	}

	

	//cout << this->processName << " " << "declared " << varTable->find(varName)->first << " = " << varTable->find(varName)->second << "\n"; // COMMENT OUT. FOR TESTING

	busyWait(); // Simulate CPU cycle delay after execution
}