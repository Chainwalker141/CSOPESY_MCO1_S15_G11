#include "ReadCommand.h"
#include "ConsoleManager.h"

ReadCommand::ReadCommand(string processName, string address, string varName, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay)
	: ICommand(processName, ICommand::READ, delay) {
	this->varName = varName; 
	this->address = address;
	this->value = 0;
	this->varTable = varTable;
}

size_t ReadCommand::hexStringToDecimal(const std::string& hexStr) {
	size_t decimalValue = 0;
	std::stringstream ss;
	ss << std::hex << hexStr;
	ss >> decimalValue;
	return decimalValue;
}

void ReadCommand::execute() {
	auto screenMap = ConsoleManager::getInstance()->getScreenMap();
	auto screen = screenMap.find(this->processName);

	if (screen != screenMap.end()) {
		auto console = screen->second;
		auto keyVal = varTable->find(address);

		// Check if symbol table pages are loaded 
		size_t pages = console->getSymbolTablePages();

		for (int i = 0; i < pages; i++) {
			if (!console->isPageLoaded(i)) {
				// LOAD PAGE
			}
		}

		// Check existence in table
		if (keyVal != varTable->end()) {
			value = keyVal->second;
			(*varTable)[varName] = value;
			std::string printLog = "[" + ConsoleManager::getCurrentTimeStamp() + "] " +
				"Read address (" + keyVal->first + ") with value: " + std::to_string(value) + " and stored to " + varName;
			console->appendOutput(printLog);
			return;
		}

		// Check if address accessed is within memory boundaries 
		size_t memory_accessed = hexStringToDecimal(address);

		std::shared_ptr<std::vector<PageInfo>> pageTable = console->getPageTable(); // Retrieve page table
		int currentPage = console->getCurrentPage();
		if ( currentPage < 0) { // If current instruction exceeds memory allocation, ignore
			// return 
			std::string printLog = "[" + ConsoleManager::getCurrentTimeStamp() + "] " +
				"Failed to read to address (" + address + "). Memory allocation full; instruction ignored";
			console->appendOutput(printLog);
			return;
		}

		// If address accessed is within memory boundaries
		if (memory_accessed >= (*pageTable)[currentPage].startByte && memory_accessed <= (*pageTable)[currentPage].endByte) {
			uint16_t valueStored = ConsoleManager::getInstance()->readAddress(address, processName);
			(*varTable)[varName] = valueStored;
			return;
		}

		// Doesnt exist, check if there is space. IDK if the address is initialized to 0 if it doesn't exist
		if (varTable->size() >= 32) {
			std::string printLog = "[" + ConsoleManager::getCurrentTimeStamp() + "] " +
				"Failed to read to address (" + address + "). Symbol table full; instruction ignored";
			console->appendOutput(printLog);
			return;
		}

		// Append value to symbol table and read/write space
		this->value = ConsoleManager::getInstance()->readAddress(address, processName);
		(*varTable)[varName] = this->value;
	}
}