#include "SubCommand.h"
#include "ConsoleManager.h"
#include <iostream> // REMOVE. ONLY FOR TESTING

SubCommand::SubCommand(string processName, string diffVar, int val1, int val2,
    std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay)
    : ICommand(processName, ICommand::SUBTRACT, delay)
{
    this->diffVar = diffVar;
    this->val1 = val1;
    this->val2 = val2;
    this->varTable = varTable;
    this->var1IsString = false;
    this->var2IsString = false;
}

SubCommand::SubCommand(string processName, string diffVar, string var1, int val2,
    std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay)
    : ICommand(processName, ICommand::SUBTRACT, delay)
{
    this->diffVar = diffVar;
    this->var1 = var1;
    this->val2 = val2;
    this->varTable = varTable;
    this->var1IsString = true;
    this->var2IsString = false;
}

SubCommand::SubCommand(string processName, string diffVar, int val1, string var2,
    std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay)
    : ICommand(processName, ICommand::SUBTRACT, delay)
{
    this->diffVar = diffVar;
    this->val1 = val1;
    this->var2 = var2;
    this->varTable = varTable;
    this->var1IsString = false;
    this->var2IsString = true;
}

SubCommand::SubCommand(string processName, string diffVar, string var1, string var2,
    std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay)
    : ICommand(processName, ICommand::SUBTRACT, delay)
{
    this->diffVar = diffVar;
    this->var1 = var1;
    this->var2 = var2;
    this->varTable = varTable;
    this->var1IsString = true;
    this->var2IsString = true;
}

void SubCommand::execute() {
    auto screenMap = ConsoleManager::getInstance()->getScreenMap();
    auto screen = screenMap.find(this->processName);

    if (screen != screenMap.end()) {
        auto console = screen->second;

        int diff = 0;
        int minuend = 0;
        int subtrahend = 0;

        // Check if diffVar has value and put it in diff
        auto diffVarTable = varTable->find(this->diffVar);

        // Check if first value given is a variable
        if (var1IsString && var1 != "") {
            auto key = varTable->find(this->var1);

            // Check variable table
            if (key != varTable->end()) {
                minuend = key->second;
            }
            else {
                varTable->insert({ var1, 0 }); // Define variable with 0 if not previously declared
            }
        }
        else {
            minuend = val1;
        }

        // Check second value if variable
        if (var2IsString && var2 != "") {
            auto key = varTable->find(this->var2);

            // Check variable table
            if (key != varTable->end()) {
                subtrahend = key->second;
            }
            else {
                varTable->insert({ var2, 0 }); // Define variable with 0 if not previously declared
            }
        }
        else {
            subtrahend = val2;
        }

        // Do operation
        diff = minuend - subtrahend;

        // Check if diffVar variable exists in varTable. If true, replace value, if not insert value.
        if (diffVarTable != varTable->end()) {
            (*varTable)[diffVar] = diff;
        }
        else {
            varTable->insert({ diffVar, diff });
        }
        varTable->insert({ diffVar, diff });
        //cout << this->processName << " Diff: " << varTable->find(diffVar)->second; // COMMENT OUT. FOR TESTING

        std::string operand1 = var1IsString ? var1 : std::to_string(val1);
        std::string operand2 = var2IsString ? var2 : std::to_string(val2);

        std::string printLog = "[" + ConsoleManager::getCurrentTimeStamp() + "] " +
            "Subtracted " + operand2 + " from " + operand1 + " to get " + std::to_string(diff) +
            ", stored in variable: " + diffVar;

        console->appendOutput(printLog);
    }

    busyWait(); // Simulate CPU cycle delay after execution
}
