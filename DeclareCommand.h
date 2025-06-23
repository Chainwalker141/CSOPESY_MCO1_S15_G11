#pragma once
#include "ICommand.h"
#include <cstdint>
#include <unordered_map>
#include <memory>

using namespace std;

class DeclareCommand : public ICommand {
public:
	DeclareCommand(string processName, string varName, uint16_t value, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable);
	void execute() override; 

private:
	string varName;
	uint16_t value; 
	std::shared_ptr<std::unordered_map < string, uint16_t>> varTable;
};