#include "PrintCommand.h"
#include "ConsoleManager.h"
#include <iostream>

PrintCommand::PrintCommand(string processName, string messageToPrint, std::shared_ptr<std::unordered_map<string, uint16_t>> varTable,int delay)
	: ICommand(processName, ICommand::PRINT, delay)
{
	this->messageToPrint = messageToPrint;
	this->varTable = varTable;
}

PrintCommand::PrintCommand(std::string processName, std::vector<std::string> expressionParts,
	std::shared_ptr<std::unordered_map<std::string, uint16_t>> varTable, int delay)
	: ICommand(processName, ICommand::PRINT, delay), varTable(varTable)
{
	this->expressionParts = std::move(expressionParts);
}

void PrintCommand::execute() {
    auto screenMap = ConsoleManager::getInstance()->getScreenMap();
    auto screen = screenMap.find(this->processName);

    if (screen != screenMap.end()) {
        auto console = screen->second;
        auto varTable = console->getVarTable();
        std::string finalMsg;

        if (!expressionParts.empty()) {
            for (const auto& part : expressionParts) {
                // If variable exists in varTable, print its value
                if (varTable->find(part) != varTable->end()) {
                    finalMsg += std::to_string(varTable->at(part));
                }
                // Otherwise treat it as literal (strip outer quotes if needed)
                else {
                    std::string cleaned = part;
                    if (!cleaned.empty() && cleaned.front() == '"' && cleaned.back() == '"') {
                        cleaned = cleaned.substr(1, cleaned.length() - 2);
                    }
                    finalMsg += cleaned;
                }
            }
        }
        else {
            auto it = varTable->find(this->messageToPrint);
            if (it != varTable->end()) {
                finalMsg = "Value from " + it->first + ": " + std::to_string(it->second);
            }
            else {
                finalMsg = "Hello world from " + this->processName;
            }
        }

        std::string printLog = "[" + ConsoleManager::getCurrentTimeStamp() + "] " + finalMsg;
        console->appendOutput(printLog);
    }

    busyWait(); // Simulate CPU cycle delay after execution
}