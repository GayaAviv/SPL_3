#pragma once

#include "../include/ConnectionHandler.h"
#include <string>
#include <unordered_map>
#include <vector>

// TODO: implement the STOMP protocol
class StompProtocol
{
private:
    int subscriptionId;
    int receipt;
    std::unordered_map<std::string, int> subscriptionAndIDs;
    std::unordered_map<std::string, std::vector<std::string>> sentMessages; // Map of channel -> messages

public:
    StompProtocol();

    void connect(const std::string& host, int port, const std::string& username, const std::string& password);
    void disconnect();
    void subscribe(const std::string& topic, int id);
    void unsubscribe(int id);
    void send(const std::string& topic, const std::string& message);
    void processFrame(const std::string& frame);

    // Get the messages sent to a specific channel
    const std::vector<std::string>& getMessagesForChannel(const std::string& channel) const;
    int getNextSubscriptionID();
    int getNextReceipt();
    void addSubscribe(std::string& topic, int id);

};
