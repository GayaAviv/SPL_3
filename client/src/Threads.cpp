#include "Threads.h"

std::queue<Frame> frameQueue;
std::mutex queueMutex;
std::condition_variable queueCondition;
std::atomic<bool> running(true);

KeyboardThread::KeyboardThread(StompProtocol& protocol, keyboardInput& keyboardInput, ConnectionHandler*& connectionHandler)
    : protocol(protocol), keyboardInputInstance(keyboardInput), connectionHandler(connectionHandler) {}

void KeyboardThread::operator()() {
    std::string userInput;

    while (running) {
        std::cout << "> ";
        std::getline(std::cin, userInput);

        Frame frame;

        if (userInput.rfind("login", 0) == 0) {
            // Initialize the connection handler on login
            std::string connectionDetails = userInput.substr(6); // Skip "login "
            frame = keyboardInputInstance.processLogin(connectionDetails, connectionHandler);

        } else if (userInput.rfind("report", 0) == 0) {
            std::string filePath = userInput.substr(7); // Skip "report "
            std::vector<Frame> frames = keyboardInputInstance.processReport(filePath, protocol);
            for(Frame f : frames){
                if (!f.getCommand().empty()) {
                    std::lock_guard<std::mutex> lock(queueMutex);
                    frameQueue.push(f);
                    queueCondition.notify_one(); // Notify the server thread
                }
            }
        } else if (protocol.getIsConnected()) {
            frame = keyboardInputInstance.processInput(userInput, protocol);
        }

        // Add the frame to the queue if it's valid
        if (!frame.getCommand().empty()) {
            std::lock_guard<std::mutex> lock(queueMutex);
            frameQueue.push(frame);
            queueCondition.notify_one(); // Notify the server thread
        }
    }
}

CommunicationThread::CommunicationThread(ConnectionHandler*& connectionHandler, StompProtocol& protocol, EncoderDecoder& encoderDecoder)
    : connectionHandler(connectionHandler), protocol(protocol), encoderDecoder(encoderDecoder) {}

void CommunicationThread::operator()() {
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
            if (!protocol.getIsConnected()) { 
                connectionHandler->close();
            }
        }
    }
}