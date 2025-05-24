#pragma once
#include <string>
#include <iomanip>
#include <sstream>
#include <ctime>

using namespace std;

class Console {
public:
	Console(string processName, int currentLine, int totalLine);

	void setProcessName(string processName);
	void setCurrentLine(int currentLine);
	void setTotalLine(int totalLine);
	void setTimestamp(string timestamp);

	string getProcessName();
	int getCurrentLine();
	int getTotalLine();
	string getTimestamp();

	void printContents();

private:
	string processName;
	int currentLine;
	int totalLine;
	string timestamp;
	
};