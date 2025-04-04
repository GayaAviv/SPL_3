#pragma once

#include "../include/ConnectionHandler.h"
#include "../include/Frame.h"
#include "../include/event.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>


class StompProtocol
{
private:
    int subscriptionId;
    int receipt;
    int disconectedReceipt;
    std::atomic<bool> isConnected;
    std::string user;
    std::unordered_set<std::string> channels; // Set of subscription channels
    std::unordered_map<int, std::string> exitReceipts; //Map of exit receipt -> channel
    std::unordered_map<int, std::string> subscriptionReceipts; //Map of subscription receipt -> channel
    std::unordered_map<std::string, int> subscriptionAndIDs; //Map of channel -> subscription id
    std::unordered_map<std::string, std::vector<Event>> sentMessages; // Map of channel -> messages

public:
    StompProtocol();

    void connect(const std::string& host, int port, const std::string& username, const std::string& password);
    
    void send(const std::string& topic, const std::string& message);

    void processFrame(Frame frame);

    // Get the messages sent to a specific channel
    const std::vector<Event> getMessagesForChannelAndUser(const std::string& channel, const std::string& user) const;
    int getNextSubscriptionID();
    int getNextReceipt();
    void addSubscribe(const std::string& topic, int id);
    void removeSubscription(const std::string& topic);
    int getSubscriptionsId(const std::string& topic);
    void disconnect();
    void setDisconnectReceipt(int id);
    void setExitReceipt(const std::string& topic, int receipt);
    void setSubscriptionReceipt(const std::string& topic, int receipt);
    void setUser(std::string newUser);
    std::string getUser();
    bool isSubscribe(const std::string& topic);

    void handleConnected(Frame frame);
    void handleMessage(Frame frame);
    void handleReceipt(Frame frame);
    void handleError(Frame frame);

    bool getIsConnected();

    void addEvent(std::string channel, Event event);

};
