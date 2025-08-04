#pragma once
#include <string>
#include <unordered_map>
#include <queue>
#include "ICommand.h"
#include <memory>
#include <cstdint>

using namespace std;

// struct for storing the page's physical location in bytes and if it is valid or not (in BS or not)
struct PageInfo {
	size_t startByte;
	size_t endByte;
	bool valid;

	PageInfo()
		: startByte(static_cast<size_t>(-1)),
		endByte(static_cast<size_t>(-1)),
		valid(false) {
	}
};

class Console {
public:
	Console(string processName, int currentLine, int totalLine, string timestamp, size_t mem_size);

	// GETTERS & SETTERS
	void setProcessName(string processName);
	void setCurrentLine(int currentLine);
	void setTotalLine(int totalLine);
	void setTimestamp(string timestamp);
	void setCoreID(int coreID);
	void setMemSize(size_t memSize);
	void setIsTerminated(bool terminated);

	string getProcessName();
	int getCurrentLine();
	int getTotalLine();
	string getTimestamp();
	int getCoreID();
	size_t getMemSize();
	bool getIsTerminated();

	string getContents();
	bool isProcessDone();

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

	// PAGING
	void initializePageTable(size_t totalMemory);
	void setPageInfo(size_t index, size_t start, size_t end, bool isValid);
	const std::vector<PageInfo> getPageTable();

private:
	string processName;
	int currentLine;
	size_t mem_size;
	int totalLine;
	string timestamp;
	int coreID = -1;

	// paging
	std::vector<PageInfo> pageTable;
	bool isTerminated = false;
	
	// COMMANDLIST IMPLEMENTATION
	std::shared_ptr<std::unordered_map < string, uint16_t>> varTable; // Symbol Table for variables
	std::queue<std::shared_ptr<ICommand>> commandList;
	vector<string> printOutputs;
};