#pragma once
#include "ICommand.h"
#include <unordered_map>

using namespace std;

class WriteCommand: public ICommand{
public:
	WriteCommand(string processName, string address, uint16_t value, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay); // when format is <WRITE 0x500 20> 
	WriteCommand(string processName, string address, string varName, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay); // when format is <WRITE 0x500 varName> 
	void execute() override;
	size_t hexStringToDecimal(const std::string& hexStr);
private: 
	string address;
	uint16_t value; 
	std::shared_ptr<std::unordered_map <string, uint16_t>> varTable;
	string varName;
}
;