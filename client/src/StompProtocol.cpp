#include "../include/StompProtocol.h"

StompProtocol:: StompProtocol() : subscriptionId (0), receipt(0), subscriptionAndIDs(), sentMessages() {}

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
void StompProtocol::disconnect(){
    subscriptionId = 0;

}