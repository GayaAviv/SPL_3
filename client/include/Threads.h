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
extern std::mutex queueMutex;
extern std::condition_variable queueCondition;
extern std::atomic<bool> running;


class KeyboardThread {
private:
    StompProtocol& protocol;
    keyboardInput& keyboardInputInstance;
    ConnectionHandler*& connectionHandler;

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