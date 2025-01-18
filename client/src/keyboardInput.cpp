#include "../include/keyboardInput.h"
#include "../include/Frame.h"
#include "../include/StompProtocol.h"

#include <string>
#include <iostream>
#include <sstream>

Frame processInput(const std::string& input, StompProtocol protocol){

    

    std::istringstream iss(input);
    std::string command;
    iss >> command; // Reading the first word (the command)
    std::string restOfInput;
    std::getline(iss, restOfInput); // Reading the rest of the string

    if (!restOfInput.empty() && restOfInput[0] == ' ') {
        restOfInput = restOfInput.substr(1);
    }

    if (command == "login") { // If the command starts with login
            std::cout << "Processing login command...\n";
            return processLogin(restOfInput);

        } else if (command == "join") { // If the command starts with join
            std::cout << "Processing join command...\n";
            return processJoin(restOfInput, protocol);

        } else if (command == "exit") { // If the command starts with exit
            std::cout << "Processing exit command...\n";
            return processExit(restOfInput, protocol);

        } else if (command == "report") { // If the command starts with report
            std::cout << "Processing report command...\n";
            return processReport(restOfInput, protocol);

        } else if (command == "summary") { // If the command starts with report
            std::cout << "Processing report command...\n";
            return processSummary(restOfInput, protocol);

        } else if (command == "logout") { // If the command starts with logout
            std::cout << "Processing logout command...\n";
            return processLogout(restOfInput, protocol);
        }

}

Frame processLogin(const std::string& loginInput){

    // Split the input into components: {host:port}, {username}, {password}
    size_t firstSpace = loginInput.find(' ');
    size_t secondSpace = loginInput.find(' ', firstSpace + 1);

    std::string username = loginInput.substr(firstSpace + 1, secondSpace - firstSpace - 1);
    std::string password = loginInput.substr(secondSpace + 1);

    // Create a CONNECT frame
    Frame frame("CONNECT", {{"accept-version", "1.2"},
                             {"host", "stomp.cs.bgu.ac.il"},
                             {"login", username},
                             {"passcode", password}},
                "");
    return frame;
}

Frame processJoin(const std::string& joinInput, StompProtocol protocol){
    int id = protocol.getNextSubscriptionID();
    Frame frame("SUBSCRIBE", {{"destination" , "/"+ joinInput},
                              {"id", std::to_string(id)},
                              {"receipt" , std::to_string(protocol.getNextReceipt())}},
                              "");
    protocol.addSubscribe(joinInput, id);
    return frame;
}

Frame processExit(const std::string& exitInput, StompProtocol protocol){
    int id = protocol.getSubscriptionsId(exitInput);
    Frame frame("UNSUBSCRIBE", {{"id", std::to_string(id)},
                              {"receipt" , std::to_string(protocol.getNextReceipt())}},
                              "");
    protocol.removeSubscription(exitInput);
    return frame;

}
Frame processReport(const std::string& reportInput, StompProtocol protocol){

}
Frame processSummary(const std::string& summaryInput, StompProtocol protocol){

}
Frame processLogout(const std::string& logoutInput, StompProtocol protocol){
    int receipt = protocol.getNextReceipt();
    protocol.disconnect();
    Frame frame("DISCONNECT", {{"receipt" , std::to_string(receipt)}},
                              "");
    return frame;
}