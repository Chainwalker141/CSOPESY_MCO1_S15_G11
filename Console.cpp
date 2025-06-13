#include <iostream>
#include "Console.h"
#include <fstream> // for file reading and writing TODO: DELETE AFTER ACTIVITY

using namespace std;

Console::Console(string processName, int currentLine, int totalLine, string timestamp)
{
	this->processName = processName;
	this->currentLine = currentLine;
	this->totalLine = totalLine;
	this->timestamp = timestamp;
}

void Console::setProcessName(string processName)
{
	this->processName = processName;
}

void Console::setCurrentLine(int currentLine)
{
	this->currentLine = currentLine;
}

void Console::setTotalLine(int totalLine)
{
	this->totalLine = totalLine;
}

void Console::setTimestamp(string timestamp)
{
	this->timestamp = timestamp;
}

string Console::getProcessName()
{
	return this->processName;
}

int Console::getCurrentLine()
{
	return this->currentLine;
}

int Console::getTotalLine()
{
	return this->totalLine;
}

string Console::getTimestamp()
{
	return this->timestamp;
}

void Console::printContents() {
	cout << "Process Name: " << this->processName << endl;
	cout << this->currentLine << " / " << this->totalLine << endl;
	cout << this->timestamp << endl;
}

// TEMP FUNCTIONS FOR ACTIVITY
string Console::getContents() {
	string contents = "Process Name: " + this->processName + "\n" + 
						to_string(this->currentLine) + " / " + to_string(this->totalLine) + "\n" +
						this->timestamp + "\n";
	return contents;
}

void Console::printFile(int coreID) {
	ofstream file(this->getProcessName()+".txt");

	file << "[Core " << coreID << "]";
	file << this->getContents();
	file.close();
}
