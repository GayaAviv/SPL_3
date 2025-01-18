#pragma once

#include "../include/ConnectionHandler.h"
#include "../include/Frame.h"
#include <string>
#include <unordered_map>
#include <vector>

// TODO: implement the STOMP protocol
class StompProtocol
{
private:
    int subscriptionId;
    int receipt;
    int disconectedReceipt;
    bool isConnected;
    std::unordered_map<std::string, int> subscriptionAndIDs; //Map of channel -> subscription id
    std::unordered_map<std::string, std::vector<std::string>> sentMessages; // Map of channel -> messages

public:
    StompProtocol();

    void connect(const std::string& host, int port, const std::string& username, const std::string& password);
    
    void send(const std::string& topic, const std::string& message);
    void processFrame(Frame frame);

    // Get the messages sent to a specific channel
    const std::vector<std::string>& getMessagesForChannel(const std::string& channel) const;
    int getNextSubscriptionID();
    int getNextReceipt();
    void addSubscribe(const std::string& topic, int id);
    void removeSubscription(const std::string& topic);
    int getSubscriptionsId(const std::string& topic);
    void disconnect();
    void setDisconnectReceipt(int id);
    void handleConnected(Frame frame);
    void handleMessage(Frame frame);
    void handleReceipt(Frame frame);
    void handleError(Frame frame);
    bool getIsConnected();

};
