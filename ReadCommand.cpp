#include "ReadCommand.h"
#include "ConsoleManager.h"

ReadCommand::ReadCommand(string processName, string address, string varName, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay)
	: ICommand(processName, ICommand::READ, delay) {
	this->varName = varName; 
	this->address = address;
}

void ReadCommand::execute() {
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
				"Read address (" + address + ") with value: " + std::to_string(value) + " and stored to " + varName;
			console->appendOutput(printLog);
			return;
		}

		// TODO: Check if address accessed is within memory boundaries 


		// Doesnt exist, check if there is space. IDK if the address is initialized to 0 if it doesn't exist
		/*if (varTable->size() >= 32) {
			std::string printLog = "[" + ConsoleManager::getCurrentTimeStamp() + "] " +
				"Failed to read to address (" + address + "). Symbol table full; instruction ignored";
			console->appendOutput(printLog);
		}*/

		// Append value to symbol table
		(*varTable)[varName] = 0;

	}
}