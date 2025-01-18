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

std::queue<Frame> frameQueue;               // Queue for frames to be sent
std::mutex queueMutex;                      // Mutex for thread-safe access
std::condition_variable queueCondition;    // Condition variable for notifications


// Global variable to control thread execution
std::atomic<bool> running(true);

// Thread to handle user input
void keyboardThreadFunction(StompProtocol& protocol, keyboardInput& keyboardInput, ConnectionHandler*& connectionHandler) {
    std::string userInput;

    while (running) {
        std::cout << "> ";
        std::getline(std::cin, userInput);

        Frame frame;

        if (userInput.rfind("login", 0) == 0) {
            // Initialize the connection handler on login
            std::string connectionDetails = userInput.substr(6); // Skip "login "
            frame = keyboardInput.processLogin(connectionDetails, connectionHandler);

        } else if(protocol.getIsConnected()) {
            frame = keyboardInput.processInput(userInput, protocol);
        }

        // Add the frame to the queue if it's valid
        if (!frame.getCommand().empty()) {
            std::lock_guard<std::mutex> lock(queueMutex);
            frameQueue.push(frame);
            queueCondition.notify_one(); // Notify the server thread
        }
    }
}

// Thread to handle server communication
void communicationThreadFunction(ConnectionHandler*& connectionHandler, StompProtocol& protocol, EncoderDecoder& encoderDecoder) {
    while (running && connectionHandler != nullptr) {
        // Handle sending frames
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (!frameQueue.empty()) {
                Frame frame = frameQueue.front();
                frameQueue.pop();

                // Encode the frame to a string
                std::string serializedFrame = encoderDecoder.encode(frame);

                // Send the frame to the server
                connectionHandler->sendLine(serializedFrame);

                std::cout << "Sent frame: " << serializedFrame << std::endl;
            }
        }

        // Handle receiving frames
        std::string receivedFrame;
        if (connectionHandler->getLine(receivedFrame)) {
            std::cout << "Received raw frame: " << receivedFrame << std::endl;

            // Decode the received frame
            Frame frame = encoderDecoder.decode(receivedFrame);

            // Process the decoded frame using StompProtocol
            protocol.processFrame(frame);
            if(true){//TODO : need to add a condition that checks if the connection need to be closed
                connectionHandler->close();
            }
        } 
    }
}


int main() {
    // Initialize protocol and input handler
    StompProtocol protocol;
    EncoderDecoder encoderDecoder;
    keyboardInput keyboardInput;
    ConnectionHandler* connectionHandler = nullptr; // Will be initialized during login

    // Start threads
    std::thread keyboardThread(keyboardThreadFunction, std::ref(protocol), std::ref(keyboardInput), std::ref(connectionHandler));
    std::thread communicationThread(communicationThreadFunction, std::ref(connectionHandler), std::ref(protocol), std::ref(encoderDecoder));


    // Wait for threads to finish
    keyboardThread.join();
    communicationThread.join();

    // Clean up connection handler if initialized
    if (connectionHandler != nullptr) {
        connectionHandler->close();
        delete connectionHandler;
    }

    std::cout << "Application terminated." << std::endl;
    return 0;
}
