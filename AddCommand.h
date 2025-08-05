#pragma once
#include "ICommand.h"
#include <unordered_map>
#include <memory>

using namespace std;

class AddCommand : public ICommand {
public:
	AddCommand(string processName, string sumVar, int val1, int val2, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay);
	AddCommand(string processName, string sumVar, string var1, int val2, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay);
	AddCommand(string processName, string sumVar, int val1, string var2, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay);
	AddCommand(string processName, string sumVar, string var1, string var2, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay);

	void execute() override;

private:
	uint16_t val1 = -1; // First value
	uint16_t val2 = -1; // Second value

	string sumVar = "";
	string var1 = ""; // variable name for first value
	string var2 = ""; // variable name for second value

	bool var1IsString; // boolean for checking if addends are variables
	bool var2IsString; 

	std::shared_ptr<std::unordered_map<string, uint16_t>> varTable;
};