#pragma once
#include "ICommand.h"
#include <cstdint>
#include <unordered_map>
#include <string>

using namespace std;

class PrintCommand : public ICommand {
public:
	PrintCommand(string processName, string messageToPrint, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay);
	PrintCommand(std::string processName, std::vector<std::string> expressionParts, std::shared_ptr<std::unordered_map<std::string, uint16_t>> varTable, int delay);
	void execute() override;


private:
	string messageToPrint;
	std::shared_ptr<std::unordered_map < string, uint16_t>> varTable;
	std::vector<std::string> expressionParts;
};
