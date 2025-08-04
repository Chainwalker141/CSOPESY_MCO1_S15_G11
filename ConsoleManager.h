#pragma once
#include <unordered_map>
#include "Console.h"
#include <sstream>

using namespace std;

class ConsoleManager {
public:
	ConsoleManager();
	static void initialize();

	// GETTERS & SETTERS
	static ConsoleManager* getInstance();
	void registerConsole(std::shared_ptr<Console> screenRef);
	void drawConsole(string processName);
	bool screenExists(string processName);
	void setInitialize(bool isInit);
	bool getInitialize();
	void displayProcessSmi();
	void displayProcessSmi(const std::string& processName);

	static std::string getCurrentTimeStamp();
	unordered_map<string, shared_ptr<Console>> getScreenMap();

	int getMaxIns();
	void setMaxIns(int maxIns);
	int getMinIns();
	void setMinIns(int minIns);

	// This is for generating dummy processes
	size_t generateRandBase2(size_t minVal, size_t maxVal);
	void schedulerTest(int BATCH_PROCESS_FREQ, int DELAYS_PER_EXEC, size_t MIN_MEM_PER_PROC, size_t MAX_MEM_PER_PROC, size_t MEM_PER_FRAME);

	// CommandList Generation
	void generateCommands(std::shared_ptr<Console>, int DELAYS_PER_EXEC);
	std::shared_ptr<ICommand> parseInstruction(const std::string& line, std::shared_ptr<Console> process, int delays);
	void generateUserCommands(std::shared_ptr<Console> process, const vector<string>& instructionLines, int DELAYS_PER_EXEC);

private:
	static ConsoleManager* consoleManager;
	std::unordered_map<string, std::shared_ptr<Console>> screenMap;
	bool isInit = false;
	int maxIns = 0; 
	int minIns = 0; 

	int generateRandInt(int minIns, int maxIns); //
};