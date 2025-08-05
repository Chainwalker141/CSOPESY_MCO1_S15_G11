#include <iostream>
#include "Console.h"
#include <fstream> // for file reading and writing TODO: DELETE AFTER ACTIVITY

using namespace std;

Console::Console(string processName, int currentLine, int totalLine, string timestamp, size_t mem_size)
{
    this->processName = processName;
    this->currentLine = currentLine;
    this->totalLine = totalLine;
    this->timestamp = timestamp;
    this->mem_size = mem_size;
    this->varTable = make_shared<std::unordered_map<string, uint16_t>>();
    this->pageTable = std::make_shared<std::vector<PageInfo>>(); // initialize shared_ptr
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

void Console::setCoreID(int coreID) {
	this->coreID = coreID;
}

void Console::setMemSize(size_t memSize) {
	this->mem_size = memSize;
}

void Console::setIsTerminated(bool terminated) {
	this->isTerminated = terminated;
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

int Console::getCoreID() {
	return this->coreID;
}

size_t Console::getMemSize() {
	return this->mem_size;
}

bool Console::getIsTerminated() {
	return this->isTerminated;
}

std::shared_ptr<std::unordered_map < string, uint16_t>> Console::getVarTable() {
	return this->varTable;
}

size_t Console::getMemoryUsage() const {
    size_t totalBytes = 0;
    if (pageTable) {
        for (const auto& page : *pageTable) {
            if (page.valid) {
                totalBytes += (page.endByte - page.startByte + 1);
            }
        }
    }
    return totalBytes;
}

void Console::appendOutput(const std::string& msg) {
	printOutputs.push_back(msg);
}

std::vector<std::string> Console::getOutputBuffer() const {
	return printOutputs;
}

void Console::initializePageTable(size_t MEM_PER_FRAME) {
	this->memPerFrame = MEM_PER_FRAME;
	size_t pagesNeeded = (mem_size + MEM_PER_FRAME - 1) / MEM_PER_FRAME;
	pageTable->resize(pagesNeeded);
}

bool Console::isPageLoaded(int index) {
	if (index > pageTable->size()) {
		cout << "INDEX OUT OF BOUNDS: " << index;
		return -1;
	}
	//cout << "size: " << pageTable->size() << " index: " << index; // Debugging
	return (*pageTable)[index].valid;
}

int Console::getCurrentPage() {
	// Each instruction is 2 bytes + 64 allotted for symbol table
	int currMemSize = (currentLine * 2 + 64);

	if (currMemSize >= mem_size) { // CurrentLine goes over allocated memory
		return -1; 
	}

	//cout << "currMemSize: " << currMemSize << " pageTable.size(): " << memPerFrame << "|| " << currMemSize / memPerFrame;
	return (currMemSize-1) / memPerFrame;
}


size_t Console::getSymbolTablePages() {
	const size_t SYMBOL_TABLE_SIZE = 64;
	return (SYMBOL_TABLE_SIZE + memPerFrame - 1) / memPerFrame;
}

void Console::setPageInfo(size_t index, size_t start, size_t end, bool isValid) {
	(*pageTable)[index].startByte = start;
	(*pageTable)[index].endByte = end;
	(*pageTable)[index].valid = isValid;
	cout << "Updated pageTable[" << index << "] of "
		<< processName
		<< " startByte = " << (*pageTable)[index].startByte << ", "
		<< "endByte = " << (*pageTable)[index].endByte << ", "
		<< "valid = " << ((*pageTable)[index].valid ? "true" : "false") << endl;
}

std::shared_ptr<std::vector<PageInfo>> Console::getPageTable() {
	return pageTable;
}

bool Console::isProcessDone(){
	return this->currentLine >= this->totalLine;
}

void Console::runInstruction() {
	// Check if current instruction is loaded onto memory
	int currPage = getCurrentPage();
	if (currPage < 0) {
		cout << "Process out of memory; Current Instruction: " << currentLine;
		return;			// TODO: Perform Error handling 
	}

	if (!isPageLoaded(currPage)) {
		cout << "Page not loaded";
		return;			// TODO: Perform page loading mechanism
	}

	if (!commandList.empty()) {
		commandList.front()->execute(); // Execute Current Line
		commandList.pop(); // Pop out of list 
		this->setCurrentLine(this->getCurrentLine() + 1); // Increment current instruction line
	}
	else {
		cout << "No commands left to execute for process: " << processName << endl;
	}
}

void Console::printContents() {
	cout << "Process Name: " << this->processName << endl;
	cout << this->currentLine << " / " << this->totalLine << endl;
	cout << this->timestamp << endl;

	/*
	cout << "\n--- Output ---" << endl;
	for (const string& line : printOutputs) {
		cout << line << endl;
	}
	*/
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
