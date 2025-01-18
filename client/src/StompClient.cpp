#include "../include/keyboardInput.h"
#include <iostream>
#include "../include/StompProtocol.h"

int main(int argc, char *argv[]) {

    StompProtocol protocol = StompProtocol();
	keyboardInput keyboardInput;
    std::string userInput;

    std::cout << "Enter commands (login, join, exit, report, logout):\n";

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, userInput);

        if (userInput == "logout") {
            keyboardInput.processInput(userInput, protocol);
            break;
        }

        keyboardInput.processInput(userInput, protocol);
    }
	return 0;
}