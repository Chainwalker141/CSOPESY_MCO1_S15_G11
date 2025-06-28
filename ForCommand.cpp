#include "ForCommand.h"
#include "ConsoleManager.h"

ForCommand::ForCommand(string processName, string varName, int start, int end, int delay)
	: ICommand(processName, ICommand::FOR, delay)
{
	this->varName = varName;
	this->start = start;
	this->end = end;
	this->forCommandsList = std::queue<std::shared_ptr<ICommand>>(); // Initialize the command list for the loop
}

void ForCommand::addCommand(std::shared_ptr<ICommand> command) {
	if (command != nullptr) {
		forCommandsList.push(command);
	}
}

void ForCommand::execute() {
    auto screenMap = ConsoleManager::getInstance()->getScreenMap();
    auto screen = screenMap.find(this->processName);
    if (screen != screenMap.end()) {
        auto console = screen->second;
        auto varTable = console->getVarTable();

        if (!varTable) return;

        for (int i = this->start; i < this->end; ++i) {
            (*varTable)[this->varName] = static_cast<uint16_t>(i);

            std::string iterMsg = "[" + ConsoleManager::getCurrentTimeStamp() + "] " +
                "For loop iteration: " + varName + " = " + std::to_string(i) +
                " (depth: " + std::to_string(depth) + ")";
            console->appendOutput(iterMsg);

            std::queue<std::shared_ptr<ICommand>> commandsCopy = this->forCommandsList;

            while (!commandsCopy.empty()) {
                auto cmd = commandsCopy.front();
                commandsCopy.pop();
                if (cmd) {
                    auto nestedFor = std::dynamic_pointer_cast<ForCommand>(cmd);
                    if (nestedFor) {
                        if (depth < 3) { // Only allow up to 3 levels
                            int prevDepth = nestedFor->depth;
                            nestedFor->depth = this->depth + 1;
                            nestedFor->execute(); // FOR COMMAND
                            nestedFor->depth = prevDepth; // Restore previous depth
                        }
                    }
                    else {
                        cmd->execute(); 
                    }
                }
            }
        }
    }
    busyWait();
}
