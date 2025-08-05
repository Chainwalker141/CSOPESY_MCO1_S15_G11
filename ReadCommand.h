#pragma once
#include "ICommand.h"
#include <unordered_map>

class ReadCommand : public ICommand {
public:
	ReadCommand(string processName, string address, string varName, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay);
	void execute() override;
	size_t hexStringToDecimal(const std::string& hexStr);

private:
	string address; 
	string varName;
	int value; 
	std::shared_ptr<std::unordered_map <string, uint16_t>> varTable;
};