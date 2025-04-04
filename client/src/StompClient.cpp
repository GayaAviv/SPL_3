#include <iostream>
#include <thread>
#include <atomic>
#include "../include/StompProtocol.h"
#include "../include/keyboardInput.h"
#include "../include/ConnectionHandler.h"
#include "../include/EncoderDecoder.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include "../include/CommunicationThread.h"

int main() {

    // Initialize 
    EncoderDecoder encoderDecoder;
    StompProtocol protocol;
    keyboardInput keyboardInputInstance;
    ConnectionHandler* connectionHandler = nullptr;
    CommunicationThread communicationThread (protocol, encoderDecoder);
    std::thread communicationThreadHandle;

    // Helper function to remove spaces strings
    auto trim = [](const std::string& str) {
        size_t start = str.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) {
            return std::string(""); // Use std::string explicitly
        }

        size_t end = str.find_last_not_of(" \t\n\r");
        return str.substr(start, end - start + 1);
    };

    
    // Helper function to send frame to the server
    auto sendFrame = [](Frame& frame, EncoderDecoder& encoderDecoder, ConnectionHandler*& connectionHandler) {
        // Encode the frame to a string
        std::string serializedFrame = encoderDecoder.encode(frame);

        if (!connectionHandler->sendLine(serializedFrame)) {
            connectionHandler->close();
            delete connectionHandler;
            return false;
        }

        return true;
    };
 
    while(true){

        std::string userInput;
        std::getline(std::cin, userInput);

        Frame frame;

        if (userInput.rfind("login", 0) == 0) {
            // Check if already connected
            if (protocol.getUser() != "") {
                std::cerr << "The client is already logged in, log out before trying again." << std::endl;
            }
            else{ // Initialize the connection handler and thread communiction on login
                
                std::string connectionDetails = trim(userInput.substr(5)); // Skip "login "

                if (connectionDetails.empty()) {
                    std::cerr << "login command need 3 args: {host:port} {username} {password}" << std::endl;
                }

                // Split the input into components: {host:port}, {username}, {password}
                size_t firstSpace = connectionDetails.find(' ');
                size_t secondSpace = connectionDetails.find(' ', firstSpace + 1);

                std::string hostPort = connectionDetails.substr(0, firstSpace);
                std::string username = connectionDetails.substr(firstSpace + 1, secondSpace - firstSpace - 1);
                std::string password = connectionDetails.substr(secondSpace + 1);

                // Extract host and port
                size_t colonPos = hostPort.find(':');
                std::string host = hostPort.substr(0, colonPos);
                short port = std::stoi(hostPort.substr(colonPos + 1));

                if(connectionHandler == nullptr){
                    // Create a new ConnectionHandler and connect
                    connectionHandler = new ConnectionHandler(host, port);
                    if(connectionHandler->connect()){
                        //Create new thread for communication
                        communicationThread.setConnectionHandler(connectionHandler);
                        communicationThreadHandle = std::thread(&CommunicationThread::operator(), &communicationThread);
                    } 
                }

                if(connectionHandler->isConnected()){
                    //Create CONNECT frame
                    frame = keyboardInputInstance.processLogin(username, password, protocol);
                    protocol.setUser(username);
                }
            }
        } 
        
        else if(protocol.getIsConnected()){ //In the case of an already established connection 

            if(userInput.rfind("report", 0) == 0) {
                std::string filePath = trim(userInput.substr(6)); // Skip "report "
                std::vector<Frame> frames = keyboardInputInstance.processReport(filePath, protocol);
                for(Frame f : frames){
                    if (!f.getCommand().empty()) {
                        sendFrame(f, encoderDecoder, connectionHandler);
                    }
                }

            //In the case of other commands
            } else {
                frame = keyboardInputInstance.processInput(userInput, protocol);
            }
        }

        //In case no one has made a connection
        else{ 
            std::cerr << "No user logged in, login first!" << std::endl;
        }

        // Send the frame to the server
        if (!frame.getCommand().empty()) {
            sendFrame(frame, encoderDecoder, connectionHandler);
        }     
    }

    // Clean up connection handler if initialized
    if (connectionHandler != nullptr) {
        delete connectionHandler;
    }

    std::cout << "Application terminated." << std::endl;
    return 0;
}
