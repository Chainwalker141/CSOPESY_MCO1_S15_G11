#pragma once
#include <string>
#include <unordered_map>
#include <queue>
#include "ICommand.h"
#include <memory>
#include <cstdint>

using namespace std;

class Console {
public:
	Console(string processName, int currentLine, int totalLine, string timestamp);

	// GETTERS & SETTERS
	void setProcessName(string processName);
	void setCurrentLine(int currentLine);
	void setTotalLine(int totalLine);
	void setTimestamp(string timestamp);

	string getProcessName();
	int getCurrentLine();
	int getTotalLine();
	string getTimestamp();

	string getContents();

	shared_ptr<std::unordered_map<string, uint16_t>> getVarTable(); 
	void setCommandList(std::queue<shared_ptr<ICommand>>);

	// TEMP FUNCTIONS FOR ACTIVITY
	void printFile(int coreID);
	void printContents();

	// COMMANDLIST IMPLEMENTATION
	void runInstruction(); 

	// FOR DISPLAYING PRINT OUTPUTS
	void appendOutput(const std::string& msg);
	vector<string> getOutputBuffer() const;

private:
	string processName;
	int currentLine;
	int totalLine;
	string timestamp;
	
	// COMMANDLIST IMPLEMENTATION
	std::shared_ptr<std::unordered_map < string, uint16_t>> varTable; // Symbol Table for variables
	std::queue<std::shared_ptr<ICommand>> commandList;
	vector<string> printOutputs;
};