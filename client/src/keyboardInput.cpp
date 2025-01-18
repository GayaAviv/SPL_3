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

    if (command == "join") { // If the command starts with join
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

Frame processLogin(const std::string& loginInput, ConnectionHandler*& connectionHandler){

    // Split the input into components: {host:port}, {username}, {password}
    size_t firstSpace = loginInput.find(' ');
    size_t secondSpace = loginInput.find(' ', firstSpace + 1);

    std::string hostPort = loginInput.substr(0, firstSpace);
    std::string username = loginInput.substr(firstSpace + 1, secondSpace - firstSpace - 1);
    std::string password = loginInput.substr(secondSpace + 1);

    // Extract host and port
    size_t colonPos = hostPort.find(':');
    std::string host = hostPort.substr(0, colonPos);
    short port = std::stoi(hostPort.substr(colonPos + 1));

    // Check if already connected
    if (connectionHandler != nullptr) {
        std::cerr << "The client is already logged in, log out before trying again" << std::endl;
        return Frame(); // Return an empty frame
    }

    // Create a new ConnectionHandler
    connectionHandler = new ConnectionHandler(host, port);
    if (!connectionHandler->connect()) {
        std::cerr << "Could not connect to server" << std::endl;
        delete connectionHandler;
        connectionHandler = nullptr;
        return Frame(); // Return an empty frame
    }

    // If connection is successful, create a CONNECT frame
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
    protocol.setDisconnectReceipt( receipt);
    Frame frame("DISCONNECT", {{"receipt" , std::to_string(receipt)}},
                              "");
    return frame;
}