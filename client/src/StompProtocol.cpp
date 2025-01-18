#include "../include/StompProtocol.h"

StompProtocol:: StompProtocol() : subscriptionId (0), receipt(0), disconectedReceipt(-1), isConnected(false), subscriptionAndIDs(), sentMessages() {}

int StompProtocol::getNextSubscriptionID(){
    return subscriptionId++;
}
 int StompProtocol::getNextReceipt(){
    return receipt++;
 }
void StompProtocol::addSubscribe(const std::string& topic, int id){
    subscriptionAndIDs.insert({topic , id});
}
void StompProtocol::removeSubscription(const std::string& topic){
    subscriptionAndIDs.erase(topic);
}
int StompProtocol::getSubscriptionsId(const std::string& topic){
    return subscriptionAndIDs.at(topic);
}
bool StompProtocol::getIsConnected(){
    return isConnected;
}

void StompProtocol::disconnect(){
    subscriptionId = 0;
    receipt = 0;
    subscriptionAndIDs.clear();
    sentMessages.clear();

    // TODO : need to close the soket!!!

}
void StompProtocol::setDisconnectReceipt(int id){
     disconectedReceipt = id;
}
void StompProtocol::processFrame(Frame frame){
     const std::string& command = frame.getCommand();

    if (command == "CONNECTED") {
        isConnected  = true;
    } else if (command == "MESSAGE") {
        handleMessage(frame);
    } else if (command == "RECEIPT") {
        handleReceipt(frame);
    } else if (command == "ERROR") {
        handleError(frame);
    }
}

void StompProtocol::handleMessage(Frame frame){
    // TODO
    
}
void StompProtocol::handleReceipt(Frame frame){
    std::unordered_map<std::string, std::string> headers = frame.getHeaders();
    //Only if its the receipt of disconnected we need to do somthing
    if(headers.at("receipt-id") == std::to_string(disconectedReceipt)){
        disconnect();
    }
    
}
void StompProtocol::handleError(Frame frame){
    //TODO
    
}
