#include "WriteCommand.h"
#include "ConsoleManager.h"

WriteCommand::WriteCommand(string processName, string address, uint16_t value, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay)
	: ICommand(processName, ICommand::DECLARE, delay) {
	this->address = address;
	this->value = value;
	this->varTable = varTable;
}

WriteCommand::WriteCommand(string processName, string address, string varName, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay)
	: ICommand(processName, ICommand::DECLARE, delay) {
	this->address = address;
	this->varTable = varTable;

	auto keyVal = this->varTable->find(varName); 

	if (keyVal != this->varTable->end()) {
		this->value = keyVal->second;
	}
	else {
		this->value = 0;
		cout << "WRITE command tried to access non-existing variable";
	}
}

void WriteCommand::execute() {
	auto screenMap = ConsoleManager::getInstance()->getScreenMap();
	auto screen = screenMap.find(this->processName);

	if (screen != screenMap.end()) {
		auto console = screen->second;
		auto keyVal = varTable->find(address);

		// Check if symbol table pages are loaded 
		int pages = console->getSymbolTablePages();

		for (int i = 0; i < pages; i++) {
			if (!console->isPageLoaded(i)) {
				// LOAD PAGE
			}
		}

		// Check existence in table
		if (keyVal != varTable->end()) { // Exists: replace
			keyVal->second = value;
			std::string printLog = "[" + ConsoleManager::getCurrentTimeStamp() + "] " +
				"Wrote to address (" + address + ") with value: " + std::to_string(value);
			console->appendOutput(printLog);
			return;
		}

		// TODO: Check if address accessed is within memory boundaries 

		// Doesnt exist, check if there is space
		if (varTable->size() >= 32) {
			std::string printLog = "[" + ConsoleManager::getCurrentTimeStamp() + "] " +
				"Failed to write to address (" + address + "). Symbol table full; instruction ignored";
			console->appendOutput(printLog);
		}

		// Append value to symbol table
		varTable->insert({ address, value });

	}

	busyWait();

}