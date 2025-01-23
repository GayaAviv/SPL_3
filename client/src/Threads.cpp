#include "../include/Threads.h"

std::queue<Frame> frameQueue;
std::mutex queueMutex;
std::condition_variable queueCondition;
std::atomic<bool> running(true);

KeyboardThread::KeyboardThread(StompProtocol& protocol, keyboardInput& keyboardInput, ConnectionHandler*& connectionHandler)
    : protocol(protocol), keyboardInputInstance(keyboardInput), connectionHandler(connectionHandler) {}

void KeyboardThread::operator()() {
    std::string userInput;

    while (running) {
        
        std::getline(std::cin, userInput);

        Frame frame;

        if (userInput.rfind("login", 0) == 0) {
            // Initialize the connection handler on login
            std::string connectionDetails = trim(userInput.substr(5)); // Skip "login "
            frame = keyboardInputInstance.processLogin(connectionDetails, protocol, connectionHandler);

        } else if (userInput.rfind("report", 0) == 0) {
            std::string filePath = trim(userInput.substr(6)); // Skip "report "
            std::vector<Frame> frames = keyboardInputInstance.processReport(filePath, protocol);
            for(Frame f : frames){
                if (!f.getCommand().empty()) {
                    //std::lock_guard<std::mutex> lock(queueMutex);
                    frameQueue.push(f);
                    queueCondition.notify_one(); // Notify the communication thread
                }
            }
        } else if (protocol.getIsConnected()) {
            frame = keyboardInputInstance.processInput(userInput, protocol);
        }

        // Add the frame to the queue if it's valid
        if (!frame.getCommand().empty()) {
            //std::lock_guard<std::mutex> lock(queueMutex);
            frameQueue.push(frame);
            queueCondition.notify_one(); // Notify the communication thread
        }
    }
}
std::string KeyboardThread::trim(const std::string& str){
    // Remove leading spaces
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return ""; // String is entirely whitespace
    }

    // Remove trailing spaces
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

CommunicationThread::CommunicationThread(ConnectionHandler*& connectionHandler, StompProtocol& protocol, EncoderDecoder& encoderDecoder)
    : connectionHandler(connectionHandler), protocol(protocol), encoderDecoder(encoderDecoder) {}

void CommunicationThread::operator()() {
    while (running) {
        if (connectionHandler == nullptr) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        //Checking the connection
        if (!connectionHandler->isConnected()) {
            std::cout << "ConnectionHandler is not connected. Exiting loop." << std::endl;
            running = false;
            break;
        }

        // Handle sending frames
        {
            std::unique_lock<std::mutex> lock(queueMutex);

            //Waiting for Frames
            queueCondition.wait(lock, [] { return !frameQueue.empty() || !running; });

            if (!running && frameQueue.empty()) {
                break;
            }

    
            Frame frame = frameQueue.front();
            frameQueue.pop();

            // Encode the frame to a string
            std::string serializedFrame = encoderDecoder.encode(frame);

            // Send the frame to the server
            if (!connectionHandler->sendLine(serializedFrame)) {
                std::cout << "Failed to send frame: " << serializedFrame << std::endl;
                running = false;
                std::cout << running << std::endl; //TODO
                break;
            }
        }

        // Handle receiving frames
        std::string receivedFrame;
        if (connectionHandler->getFrameAscii(receivedFrame, '\0')) {  //Remove the final character if it exists
            receivedFrame.pop_back();
            std::cout << "Full frame received: \n" << receivedFrame << std::endl;
        }
        else{ //In case of server disconnected
            std::cout << "Failed to send Frame to the server" << std::endl;
            connectionHandler->close();
            running = false;
            break;
        } 

        // Decode the received frame
        Frame frame = encoderDecoder.decode(receivedFrame);

        // Process the decoded frame using StompProtocol
        protocol.processFrame(frame);

        if (!protocol.getIsConnected()) {
            std::cout << "Protocol disconnected. Closing connection." << std::endl;
            connectionHandler->close();
            running = false;
            break;
        }
    }
}

