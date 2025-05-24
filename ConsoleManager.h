#pragma once
#include <unordered_map>
#include "Console.h"
#include <sstream>

using namespace std;

class ConsoleManager {
public:
	ConsoleManager();
	static void initialize();
	static ConsoleManager* getInstance();
	void registerConsole(std::shared_ptr<Console> screenRef);
	void drawConsole(string processName);
	bool screenExists(string processName);
	void setInitialize(bool isInit);
	bool getInitialize();

private:
	static ConsoleManager* consoleManager;
	std::unordered_map<string, std::shared_ptr<Console>> screenMap;
	bool isInit = false;
};