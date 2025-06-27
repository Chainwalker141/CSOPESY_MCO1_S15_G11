#pragma once
#include "ICommand.h"
#include <unordered_map>
#include <memory>

using namespace std;

class SubCommand : public ICommand {
public:
	SubCommand(string processName, string diffVar, int val1, int val2, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay);
	SubCommand(string processName, string diffVar, string var1, int val2, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay);
	SubCommand(string processName, string diffVar, int val1, string var2, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay);
	SubCommand(string processName, string diffVar, string var1, string var2, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay);

	void execute() override;

private:
	int val1 = -1; // First value
	int val2 = -1; // Second value

	string diffVar = "";
	string var1 = ""; // variable name for first value
	string var2 = ""; // variable name for second value

	bool var1IsString; // boolean for checking if addends are variables
	bool var2IsString;

	std::shared_ptr<std::unordered_map<string, uint16_t>> varTable;
};