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
    this->varTable = make_shared<std::unordered_map<string, uint16_t>>();
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

void Console::setCommandList(std::queue<std::shared_ptr<ICommand>> commandList) {
	this->commandList = std::move(commandList);
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

std::shared_ptr<std::unordered_map < string, uint16_t>> Console::getVarTable() {
	return this->varTable;
}

void Console::runInstruction() {
	this->commandList.front()->execute(); // Execute Current Line
	commandList.pop(); // Pop out of list 
	this->setCurrentLine(this->getCurrentLine() + 1); // Increment current instruction line
}

void Console::printContents() {
	cout << "Process Name: " << this->processName << endl;
	cout << this->currentLine << " / " << this->totalLine << endl;
	cout << this->timestamp << endl;
}

// TEMP FUNCTIONS FOR ACTIVITY
string Console::getContents() {
	string contents = " Process Name: " + this->processName + "\n" + 
						to_string(this->currentLine) + " / " + to_string(this->totalLine) + "\n" +
						this->timestamp + "\n";
	return contents;
}

void Console::printFile(int coreID) {
	ofstream file("./output/"+this->getProcessName() + ".txt", ios::app);

	file << "[Core " << coreID << "]";
	file << this->getContents();
	file << "\n";
	file.close();
}
