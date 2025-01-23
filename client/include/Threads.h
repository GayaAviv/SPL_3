#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include "StompProtocol.h"
#include "keyboardInput.h"
#include "ConnectionHandler.h"
#include "EncoderDecoder.h"

extern std::queue<Frame> frameQueue;
extern std::mutex queueMutex;  //Lock object
extern std::condition_variable queueCondition; //Implement wait and notify synchronization


class KeyboardThread {
private:
    StompProtocol& protocol;
    keyboardInput& keyboardInputInstance;
    ConnectionHandler*& connectionHandler;
    std::string trim(const std::string& str);

public:
    KeyboardThread(StompProtocol& protocol, keyboardInput& keyboardInput, ConnectionHandler*& connectionHandler);
    void operator()();
};

class CommunicationThread {
    private:
    ConnectionHandler*& connectionHandler;
    StompProtocol& protocol;
    EncoderDecoder& encoderDecoder;
    
public:
    CommunicationThread(ConnectionHandler*& connectionHandler, StompProtocol& protocol, EncoderDecoder& encoderDecoder);
    void operator()();

};