#pragma once
#include <iostream>
#include <string>
using namespace std;

string green = "\033[32m";
string reset = "\033[0m";
string yellow = "\033[38;5;229m";

void display_ASCII() {

    /*
___  ________  ___________      _____  _     _____
|  \/  |  _  ||  _  |  _  \    /  __ \| |   |_   _|
| .  . | | | || | | | | | |    | /  \/| |     | |
| |\/| | | | || | | | | | |    | |    | |     | |
| |  | \ \_/ /\ \_/ / |/ /     | \__/\| |_____| |_
\_|  |_/\___/  \___/|___/       \____/\_____/\___/
    */
    std::cout << R"(
    ___  ________  ___________      _____  _     _____
    |  \/  |  _  ||  _  |  _  \    /  __ \| |   |_   _|
    | .  . | | | || | | | | | |    | /  \/| |     | |
    | |\/| | | | || | | | | | |    | |    | |     | |
    | |  | \ \_/ /\ \_/ / |/ /     | \__/\| |_____| |_
    \_|  |_/\___/  \___/|___/       \____/\_____/\___/
		
	)" << std::endl;
    cout << green << "Hello, Welcome to MOOD CLI commandline!\n" << reset;
    cout << yellow << "Type 'exit' to quit, 'clear' to clear the screen\n" << reset;
}