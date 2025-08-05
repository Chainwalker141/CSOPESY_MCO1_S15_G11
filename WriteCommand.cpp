#include "WriteCommand.h"
#include "ConsoleManager.h"

WriteCommand::WriteCommand(string processName, string address, uint16_t value, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay)
	: ICommand(processName, ICommand::DECLARE, delay) {
	this->address = address;
	this->value = value;
	this->varTable = varTable;
	this->varName = "";
}

WriteCommand::WriteCommand(string processName, string address, string varName, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay)
	: ICommand(processName, ICommand::DECLARE, delay) {
	this->address = address;
	this->varTable = varTable;
	this->varName = varName;
	auto keyVal = this->varTable->find(varName); 

	if (keyVal != this->varTable->end()) {
		this->value = keyVal->second;
	}
	else {
		this->value = 0;
		cout << "WRITE command tried to access non-existing variable " << varName;
	}
}

size_t WriteCommand::hexStringToDecimal(const std::string& hexStr) {
	size_t decimalValue = 0;
	std::stringstream ss;
	ss << std::hex << hexStr;
	ss >> decimalValue;
	return decimalValue;
}

void WriteCommand::execute() {
	auto screenMap = ConsoleManager::getInstance()->getScreenMap();
	auto screen = screenMap.find(this->processName);

	if (screen != screenMap.end()) {
		auto console = screen->second;
		auto keyVal = varTable->find(address);
		auto varTab = varTable->find(this->varName);
		// Check if symbol table pages are loaded 
		size_t pages = console->getSymbolTablePages();

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
			//cout << "Writing1 " << value;
			return;
		}

		//if (varTab != varTable->end()) { // Exists: replace
		//	keyVal->second = value;
		//	std::string printLog = "[" + ConsoleManager::getCurrentTimeStamp() + "] " +
		//		"Wrote to address (" + address + ") with value: " + std::to_string(value);
		//	console->appendOutput(printLog);
		//	//cout << "Writing1 " << value;
		//	return;
		//}

		// Check if address accessed is within memory boundaries 
		size_t memory_accessed = hexStringToDecimal(address);

		std::shared_ptr<std::vector<PageInfo>> pageTable = console->getPageTable(); // Retrieve page table
		int currentPage = console->getCurrentPage();
		if (currentPage < 0) { // If current instruction exceeds memory allocation, ignore
			// return 
			std::string printLog = "[" + ConsoleManager::getCurrentTimeStamp() + "] " +
				"Failed to read to address (" + address + "). Memory allocation full; instruction ignored";
			console->appendOutput(printLog);
			//cout << "Writing2 " << value;
			return;
		}

		// Doesnt exist, check if there is space
		if (varTable->size() >= 32) {
			std::string printLog = "[" + ConsoleManager::getCurrentTimeStamp() + "] " +
				"Failed to write to address (" + address + "). Symbol table full; instruction ignored";
			console->appendOutput(printLog);
			//cout << "Writing3 " << value;
		}

		// Append value to symbol table and the Read/Write space
		ConsoleManager::getInstance()->writeToAddress(address, this->processName, this->value);
		if (varTab != varTable->end()) {
			(*varTable)[address] = varTab->second;
		}
		else {
			(*varTable)[address] = this->value;
		}
		//cout << "Writing4 " << this->value << address;
		// screenMap-><
	}

	busyWait();

}