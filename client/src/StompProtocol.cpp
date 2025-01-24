#include "../include/StompProtocol.h"

std::mutex sentMessagesMutex;
std::mutex subscriptionMutex;
std::mutex receiptMutex;

StompProtocol:: StompProtocol() : subscriptionId (0), receipt(0), disconectedReceipt(-1), isConnected(false), user(""),
                                 exitReceipts(), subscriptionReceipts(), subscriptionAndIDs(), sentMessages(),channels() {}

int StompProtocol::getNextSubscriptionID(){
    return subscriptionId++;
}
 int StompProtocol::getNextReceipt(){
    return receipt++;
 }
void StompProtocol::addSubscribe(const std::string& topic, int id){
    std::lock_guard<std::mutex> lock(subscriptionMutex);
    subscriptionAndIDs.insert({topic , id});
    channels.insert(topic);
}
void StompProtocol::removeSubscription(const std::string& topic){
    std::lock_guard<std::mutex> lock(subscriptionMutex);
    subscriptionAndIDs.erase(topic);
    channels.erase(topic);
}
int StompProtocol::getSubscriptionsId(const std::string& topic){
    std::lock_guard<std::mutex> lock(subscriptionMutex);
    return subscriptionAndIDs.at(topic);
}
bool StompProtocol::getIsConnected(){
    return isConnected.load();
}

bool StompProtocol::isSubscribe(const std::string& topic){
    std::lock_guard<std::mutex> lock(subscriptionMutex);
    return channels.find(topic) != channels.end();
}

void StompProtocol::setExitReceipt(const std::string& topic, int receipt){
    std::lock_guard<std::mutex> lock(receiptMutex);
    exitReceipts.insert({ receipt, topic});
}
void StompProtocol::setSubscriptionReceipt(const std::string& topic, int receipt){
    std::lock_guard<std::mutex> lock(receiptMutex);
    subscriptionReceipts.insert({ receipt, topic});
}
void StompProtocol::setUser(std::string newUser){
    user = newUser;
}
std::string StompProtocol::getUser(){
    return user;
}

void StompProtocol::disconnect(){
    subscriptionId = 0;
    receipt = 0;
    disconectedReceipt = -1;
    subscriptionAndIDs.clear();
    sentMessages.clear();
    exitReceipts.clear();
    subscriptionReceipts.clear();
    user ="";
    isConnected.store(false);
}

const std::vector<Event> StompProtocol::getMessagesForChannelAndUser(const std::string& channel, const std::string& user) const{

    std::lock_guard<std::mutex> lock(sentMessagesMutex);
    std::vector<Event> filteredEvents;

    // Check if the channel exists in the map
    auto it = sentMessages.find(channel);
    if (it != sentMessages.end()) {
        // Iterate through the events in the vector for the given channel
        const std::vector<Event>& events = it->second;
        for (const Event& event : events) {
            // Filter events by user
            if (event.getEventOwnerUser() == user) {
                filteredEvents.push_back(event);
            }
        }
    }
    return filteredEvents;
}

void StompProtocol::addEvent(std::string channel, Event event){

    std::lock_guard<std::mutex> lock(sentMessagesMutex);
    // Check if the key exists in the map
    if (sentMessages.find(channel) != sentMessages.end()) {
        // If the key exists, insert the event in the correct position in the vector
        auto& events = sentMessages[channel];
        auto it = std::lower_bound(events.begin(), events.end(), event,
                                   [](const Event& a, const Event& b) {
                                       return a.get_date_time() < b.get_date_time();
                                   });
        events.insert(it, event);
    } else {
        // If the key does not exist, create a new vector and add the event
        sentMessages[channel] = std::vector<Event>{event};
    }
}

void StompProtocol::setDisconnectReceipt(int id){
     disconectedReceipt = id;
}
void StompProtocol::processFrame(Frame frame){

     const std::string& command = frame.getCommand();

    if (command == "CONNECTED") {
        handleConnected(frame);
    } else if (command == "MESSAGE") {
        handleMessage(frame);
    } else if (command == "RECEIPT") {
        handleReceipt(frame);
    } else if (command == "ERROR") {
        handleError(frame);
    }
}
void StompProtocol::handleConnected(Frame frame){
    isConnected.store(true);
    std::cout << "Login successful" << std::endl;
}

void StompProtocol::handleMessage(Frame frame){
    std::string channle = frame.getHeaders().at("destination");
    // Add the massage to the list
    const std::string frameBody = frame.getBody();
    Event newEvent(frameBody, channle);
    
    std::string channel = newEvent.get_channel_name();
    addEvent(channel, newEvent);    
    std::cout << "reported" << std::endl;
}
void StompProtocol::handleReceipt(Frame frame){
    std::unordered_map<std::string, std::string> headers = frame.getHeaders();

    //Only if its the receipt of disconnected we need to do somthing
    if(headers.at("receipt-id") == std::to_string(disconectedReceipt)){
        disconnect();
        std::cout << "Logged out" << std::endl;
    }

    // If its a exit recipt
    for (const auto& pair : exitReceipts) {
        int receipt = pair.first;         // Key (int)
        if(headers.at("receipt-id") == std::to_string(receipt)){
            const std::string& value = pair.second; // Value (std::string)
            removeSubscription(value);
            std::cout << "Exited channel " << value << std::endl;
        }
    }

    //if its a subscribe receipt
    for (const auto& pair : subscriptionReceipts) {
        int receipt = pair.first;         // Key (int)
        if(headers.at("receipt-id") == std::to_string(receipt)){
            const std::string& value = pair.second; // Value (std::string)
            addSubscribe(value, receipt);
            std::cout << "Joined channel " << value << std::endl;
        }
    }  
}
void StompProtocol::handleError(Frame frame){
    std::cout << "Received error from the server: " << std::endl;
    std::cout << "\n";
    std::cout  << frame.toString() << std::endl;
    disconnect();
}
