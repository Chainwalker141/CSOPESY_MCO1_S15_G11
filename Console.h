#pragma once
#include <string>

using namespace std;

class Console {
public:
	Console(string processName, int currentLine, int totalLine, string timestamp);

	void setProcessName(string processName);
	void setCurrentLine(int currentLine);
	void setTotalLine(int totalLine);
	void setTimestamp(string timestamp);

	string getProcessName();
	int getCurrentLine();
	int getTotalLine();
	string getTimestamp();

	void printContents();

	// TEMP FUNCTIONS FOR ACTIVITY
	string getContents();
	void printFile(int coreID);

private:
	string processName;
	int currentLine;
	int totalLine;
	string timestamp;
	
};