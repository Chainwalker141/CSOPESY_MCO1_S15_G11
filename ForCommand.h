#pragma once
#include "ICommand.h"
#include <unordered_map>
#include <string>
#include <queue>

using namespace std;

class ForCommand : public ICommand {
public:
	ForCommand(string processName, string varName, int start, int end, int delay);
	void addCommand(std::shared_ptr<ICommand>);
	void execute() override;

private:
	string messageToPrint;
	string varName; // Variable name to store the loop index
	int start; // Start value of the loop
	int end; // End value of the loop
	std::queue<std::shared_ptr<ICommand>> forCommandsList;
	int depth = 3; // Depth of the loop, can be used to limit the number of iterations or for nested loops

};

