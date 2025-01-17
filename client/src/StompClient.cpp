#include "../include/keyboardInput.h"
#include <iostream>

int main(int argc, char *argv[]) {
	
	keyboardInput keyboardInput;
    std::string userInput;

    std::cout << "Enter commands (login, join, exit, report, logout):\n";

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, userInput);

        if (userInput == "logout") {
            keyboardInput.processInput(userInput);
            break;
        }

        keyboardInput.processInput(userInput);
    }
	return 0;
}