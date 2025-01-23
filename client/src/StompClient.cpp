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
#include "../include/Threads.h"

int main() {
    // Initialize protocol and input handler
    StompProtocol protocol;
    EncoderDecoder encoderDecoder;
    keyboardInput keyboardInput;
    ConnectionHandler* connectionHandler = nullptr; // Will be initialized during login

    // Create threads for keyboard and communication
    KeyboardThread keyboardThread(protocol, keyboardInput, connectionHandler);
    CommunicationThread communicationThread(connectionHandler, protocol, encoderDecoder);

    std::thread keyboardThreadHandle(keyboardThread);
    std::thread communicationThreadHandle(communicationThread);

    // Wait for threads to finish
    keyboardThreadHandle.join();
    communicationThreadHandle.join();


    // Clean up connection handler if initialized
    if (connectionHandler != nullptr) {
        delete connectionHandler;
    }

    std::cout << "Application terminated." << std::endl;
    return 0;
}
