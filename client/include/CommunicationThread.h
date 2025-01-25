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

class CommunicationThread {
    private:
    ConnectionHandler* connectionHandler;
    StompProtocol& protocol;
    EncoderDecoder& encoderDecoder;
    
public:
    
    CommunicationThread(StompProtocol& protocol, EncoderDecoder& encoderDecoder);
    void setConnectionHandler(ConnectionHandler* handler);
    void operator()();
};