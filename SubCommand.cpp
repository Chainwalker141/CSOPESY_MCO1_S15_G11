#include "SubCommand.h"
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
    int diff = 0;
    int minuend = 0;
    int subtrahend = 0;

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

    diff = minuend - subtrahend;

    varTable->insert({ diffVar, diff });
    //cout << this->processName << " Diff: " << varTable->find(diffVar)->second; // COMMENT OUT. FOR TESTING

    busyWait(); // Simulate CPU cycle delay after execution
}
