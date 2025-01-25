#include "../include/CommunicationThread.h"

bool running(true);


CommunicationThread::CommunicationThread( StompProtocol& protocol, EncoderDecoder& encoderDecoder)
    : connectionHandler(nullptr), protocol(protocol), encoderDecoder(encoderDecoder) {}

void CommunicationThread::setConnectionHandler(ConnectionHandler* handler) {
        connectionHandler = handler;
    }

void CommunicationThread::operator()() {
    while (running) { 

        // Handle receiving frames
        std::string receivedFrame;
        if (connectionHandler->getFrameAscii(receivedFrame, '\0')) { 
            receivedFrame.pop_back();
        }
        else{ //In case of server disconnected
            connectionHandler->close();
            running = false;
            break;
        } 

        // Decode the received frame
        Frame frame = encoderDecoder.decode(receivedFrame);

        // Process the decoded frame using StompProtocol
        protocol.processFrame(frame);
        if(frame.getCommand() == "ERROR"){
            connectionHandler->close();
            running = false;
        }
        else if(!protocol.getIsConnected()){
            running = false;
        }
    }
}