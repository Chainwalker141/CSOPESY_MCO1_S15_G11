#include "AddCommand.h"
#include "ConsoleManager.h"
#include <iostream> // REMOVE. ONLY FOR TESTING

AddCommand::AddCommand(string processName, string sumVar, int val1, int val2,
    std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay)
    : ICommand(processName, ICommand::ADD, delay)
{
    this->sumVar = sumVar;
    this->val1 = val1;
    this->val2 = val2;
    this->varTable = varTable;
    this->var1IsString = false;
    this->var2IsString = false;
}

AddCommand::AddCommand(string processName, string sumVar, string var1, int val2,
    std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay)
    : ICommand(processName, ICommand::ADD, delay)
{
    this->sumVar = sumVar;
    this->var1 = var1;
    this->val2 = val2;
    this->varTable = varTable;
    this->var1IsString = true;
    this->var2IsString = false;
}

AddCommand::AddCommand(string processName, string sumVar, int val1, string var2,
    std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay)
    : ICommand(processName, ICommand::ADD, delay)
{
    this->sumVar = sumVar;
    this->val1 = val1;
    this->var2 = var2;
    this->varTable = varTable;
    this->var1IsString = false;
    this->var2IsString = true;
}

AddCommand::AddCommand(string processName, string sumVar, string var1, string var2,
    std::shared_ptr<std::unordered_map<string, uint16_t>> varTable, int delay)
    : ICommand(processName, ICommand::ADD, delay)
{
    this->sumVar = sumVar;
    this->var1 = var1;
    this->var2 = var2;
    this->varTable = varTable;
    this->var1IsString = true;
    this->var2IsString = true;
}

void AddCommand::execute() {
    auto screenMap = ConsoleManager::getInstance()->getScreenMap();
    auto screen = screenMap.find(this->processName);

    if (screen != screenMap.end()) {
        auto console = screen->second;

        int sum = 0;
        int addend1 = 0;
        int addend2 = 0;

        // Check if sumVar has value and exists

        auto sumVarTable = varTable->find(this->sumVar);

       /* if (sumVarTable != varTable->end()) {
            sum = sumVarTable->second;
        }*/


        // Check if first value given is a variable
        if (var1IsString && var1 != "") {
            auto key = varTable->find(this->var1);

            // Check variable table
            if (key != varTable->end()) {
                addend1 = key->second;
            }
            else {
                addend1 = 0;
                varTable->insert({ var1, addend1 }); // Define variable with 0 if not previously declared
            }
        }
        else {
            addend1 = val1;
        }

        // Check second value if variable
        if (var2IsString && var2 != "") {
            auto key = varTable->find(this->var2);

            // Check variable table
            if (key != varTable->end()) {
                addend2 = key->second;
            }
            else {
                addend2 = 0;
                varTable->insert({ var2, addend2 }); // Define variable with 0 if not previously declared
            }
        }
        else {
            addend2 = val2;
        }

        // Do operation
        sum = addend1 + addend2;
        cout << "X = " << sum;
        // If sumVar variable exists in varTable, replace value with sum. Otherwise, insert
        if (sumVarTable != varTable->end())
            (*varTable)[sumVar] = sum;
        else {
            varTable->insert({ sumVar, sum });
        }

        //cout << this->processName << " Sum: " << varTable->find(sumVar)->second; // COMMENT OUT. sFOR TESTING

        std::string operand1 = var1IsString ? var1 : std::to_string(val1);
        std::string operand2 = var2IsString ? var2 : std::to_string(val2);

        std::string printLog = "[" + ConsoleManager::getCurrentTimeStamp() + "] " +
            "Added " + operand1 + " and " + operand2 + " to get " + std::to_string(sum) +
            ", stored in variable: " + sumVar;

        console->appendOutput(printLog);
    }


    busyWait(); // Simulate CPU cycle delay after execution
}
