#include "../include/StompProtocol.h"

StompProtocol:: StompProtocol() : subscriptionId (0), receipt(0), subscriptionAndIDs(), sentMessages() {}

int StompProtocol::getNextSubscriptionID(){
    return subscriptionId++;
}
 int StompProtocol::getNextReceipt(){
    return receipt++;
 }
  void StompProtocol::addSubscribe(std::string& topic, int id){
    subscriptionAndIDs.insert({topic , id});
  }
