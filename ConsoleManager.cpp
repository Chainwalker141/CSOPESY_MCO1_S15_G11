#include "ConsoleManager.h"

using namespace std;

ConsoleManager* ConsoleManager::consoleManager = nullptr;

ConsoleManager::ConsoleManager() {
};

void ConsoleManager::initialize() {
    consoleManager = new ConsoleManager();
}

ConsoleManager* ConsoleManager::getInstance()
{
    return consoleManager;
}

void ConsoleManager::registerConsole(shared_ptr<Console> screenRef) {
    this->screenMap[screenRef->getProcessName()] = screenRef;
}

void ConsoleManager::drawConsole(string processName) {
    this->screenMap[processName]->printContents();
}