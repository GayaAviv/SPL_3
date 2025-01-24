#include "../include/Threads.h"

std::queue<Frame> frameQueue;
std::mutex queueMutex;
std::condition_variable queueCondition;
std::atomic<bool> running(true);

KeyboardThread::KeyboardThread(StompProtocol& protocol, keyboardInput& keyboardInput, ConnectionHandler*& connectionHandler)
    : protocol(protocol), keyboardInputInstance(keyboardInput), connectionHandler(connectionHandler) {}

void KeyboardThread::operator()() {
    std::string userInput;

    while (running) { //While no one did login or the connection not lost
        
        std::getline(std::cin, userInput);

        Frame frame;

        if (userInput.rfind("login", 0) == 0) {
            // Check if already connected
            if (connectionHandler != nullptr) {
                std::cerr << "The client is already logged in, log out before trying again." << std::endl;
                break;
            }
            else{    
                // Initialize the connection handler on login
                std::string connectionDetails = trim(userInput.substr(5)); // Skip "login "
                frame = keyboardInputInstance.processLogin(connectionDetails, protocol, connectionHandler);
            }
        } 
        
        if(protocol.getIsConnected()){ //In the case of an already established connection 

            if(userInput.rfind("report", 0) == 0) {
                std::string filePath = trim(userInput.substr(6)); // Skip "report "
                std::vector<Frame> frames = keyboardInputInstance.processReport(filePath, protocol);
                for(Frame f : frames){
                    if (!f.getCommand().empty()) {
                        //std::lock_guard<std::mutex> lock(queueMutex);
                        frameQueue.push(f);
                        queueCondition.notify_one(); // Notify the communication thread
                    }
                }

            } else {
                frame = keyboardInputInstance.processInput(userInput, protocol);
            }

            // Add the frame to the queue if it's valid
            if (!frame.getCommand().empty()) {
                //std::lock_guard<std::mutex> lock(queueMutex);
                frameQueue.push(frame);
                queueCondition.notify_one(); // Notify the communication thread
            }
        }

        else{ //In case no one has made a connection
            std::cerr << "No user logged in, login first!" << std::endl;
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

        // Handle sending frames
        {
             std::unique_lock<std::mutex> lock(queueMutex);

            //Waiting for Frames
            queueCondition.wait(lock, [] { return !frameQueue.empty() || !running; });

            if (!connectionHandler->isConnected() && frameQueue.empty()) {
                break;
            }

            Frame frame = frameQueue.front();
            frameQueue.pop();

            // Encode the frame to a string
            std::string serializedFrame = encoderDecoder.encode(frame);

            // Send the frame to the server
            if (!connectionHandler->sendLine(serializedFrame)) {
                connectionHandler->close();
                running.store(false);
                break;
            }
                   
        }

        // Handle receiving frames
        std::string receivedFrame;
        if (connectionHandler->getFrameAscii(receivedFrame, '\0')) { 
            receivedFrame.pop_back();
        }
        else{ //In case of server disconnected
            connectionHandler->close();
            running.store(false);
            break;
        } 

        // Decode the received frame
        Frame frame = encoderDecoder.decode(receivedFrame);

        // Process the decoded frame using StompProtocol
        protocol.processFrame(frame);
        if(frame.getCommand() == "ERROR"){
            connectionHandler->close();
            running.store(false);
        }
        else if(!protocol.getIsConnected()){
            connectionHandler = nullptr;
        }
    }
}