#include "../include/keyboardInput.h"
#include "../include/Frame.h"
#include "../include/StompProtocol.h"
#include "../include/event.h"

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <ctime>

Frame keyboardInput::processInput(const std::string& input, StompProtocol& protocol){

    std::istringstream iss(input);
    std::string command;
    iss >> command; // Reading the first word (the command)
    std::string restOfInput;
    std::getline(iss, restOfInput); // Reading the rest of the string

    if (!restOfInput.empty() && restOfInput[0] == ' ') {
        restOfInput = restOfInput.substr(1);
    }

    if (command == "join") { // If the command starts with join
            return processJoin(restOfInput, protocol);

        } else if (command == "exit") { // If the command starts with exit
            return processExit(restOfInput, protocol);

        } else if (command == "summary") { // If the command starts with report
            processSummary(restOfInput, protocol);
            return Frame();

        } else if (command == "logout") { // If the command starts with logout
            return processLogout(restOfInput, protocol);
        }
    std::cout << "Illegal command, try again\n";
    return Frame();
}

Frame keyboardInput::processLogin(const std::string& username,const std::string& password, StompProtocol& protocol){
       
    Frame frame("CONNECT", {{"accept-version", "1.2"},
                                {"host", "stomp.cs.bgu.ac.il"},
                                {"login", username},
                                {"passcode", password}},
                    "");
    return frame;
}

Frame keyboardInput::processJoin(const std::string& joinInput, StompProtocol& protocol){
    
    if (joinInput.empty()) { //In the case of illegal arguments
        std::cerr << "join command need 1 args: {channel_name}" << std::endl;
        return Frame(); // Return an empty frame or handle the error as needed
    }

    if(protocol.isSubscribe(joinInput)){ //In the case of double subscription
        std::cerr << "Already subscribed to this topic. Please try again." << std::endl;
        return Frame(); 
    }

    int id = protocol.getNextSubscriptionID();
    int receipt = protocol.getNextReceipt();

    Frame frame("SUBSCRIBE", {{"destination" , "/"+joinInput},
                              {"id", std::to_string(id)},
                              {"receipt" , std::to_string(receipt)}},
                              "");

    protocol.setSubscriptionReceipt(joinInput, receipt);
    
    return frame;
}

Frame keyboardInput::processExit(const std::string& exitInput, StompProtocol& protocol){

    if (exitInput.empty()) { //In the case of illegal arguments
        std::cerr << "exit command need 1 args: {channel_name}" << std::endl;
        return Frame();
    }

    if(!protocol.isSubscribe(exitInput)){ //In the case of no subscription to exit
        std::cerr << "You are not subscribed to channel " << exitInput << std::endl;
        return Frame(); 
    }
    
    int id = protocol.getSubscriptionsId(exitInput);
    int receipt = protocol.getNextReceipt();
    Frame frame("UNSUBSCRIBE", {{"id", std::to_string(id)},
                              {"receipt" , std::to_string(receipt)}},
                              "");
    protocol.setExitReceipt(exitInput, receipt);
    
    return frame;

}
std::vector<Frame> keyboardInput::processReport(const std::string& reportInput, StompProtocol& protocol){
    std::vector<Frame> frames;

    if (reportInput.empty()) { //In the case of illegal arguments
        std::cerr << "report command need 1 args: {file}" << std::endl;
        return frames; // Return an empty frame or handle the error as needed
    }
    
    //read the file path and prase the channel name and event it contains
    names_and_events channel_events = parseEventsFile(reportInput);
    std::string channel = channel_events.channel_name;
    std::vector<Event> events =  channel_events.events;
    std::string user = protocol.getUser();
    //save each event on the client
    for(Event e : events){
        e.setEventOwnerUser(user);
        std::string body = "user:" + e.getEventOwnerUser() + "\n" +
                           "city: " + e.get_city() +"\n" +
                           "event name: " + e.get_name() +"\n" +
                           "date time: " + std::to_string(e.get_date_time()) + "\n" +
                           "general information:\n" +
                                                " active: " + e.get_general_information().at("active") + "\n" +
                                                " forces_arrival_at_scene: " + e.get_general_information().at("forces_arrival_at_scene") +"\n" +
                            "description:\n" +
                             e.get_description();

        //send frame
        Frame frame("SEND", {{"destination" , "/"+channel}},
                                body);
        frames.push_back(frame);
    }
    return frames;
    
}

Frame keyboardInput::processSummary(const std::string& summaryInput, StompProtocol& protocol){
   
    if (summaryInput.empty()) { //In the case of illegal arguments
        std::cerr << "summary command need 3 args: {channel_name} {user} {file}" << std::endl;
        return Frame(); // Return an empty frame or handle the error as needed
    }

    // Split the input string to extract parameters
    std::istringstream inputStream(summaryInput);
    std::string channelName, user, fileName;

    // Parse the command and parameters
    inputStream >> channelName >> user >> fileName;

    // Retrieve the events for the given channel and user from the protocol
    const std::vector<Event>& events = protocol.getMessagesForChannelAndUser(channelName, user); 

    // Call the function to write the summary to the file
    writeSummary(channelName, user, fileName, events);

    return Frame();

}

Frame keyboardInput::processLogout(const std::string& logoutInput, StompProtocol& protocol){
    int receipt = protocol.getNextReceipt();
    protocol.setDisconnectReceipt(receipt);
    Frame frame("DISCONNECT", {{"receipt" , std::to_string(receipt)}},
                              "");
    return frame;
}

void keyboardInput::writeSummary(const std::string& channelName, const std::string& user, const std::string& fileName, const std::vector<Event>& events) {
    std::ofstream outFile(fileName);

    if (!outFile) {
        std::cerr << "Error: Could not open file " << fileName << " for writing.\n";
        return;
    }

    // Filter events by channelName and user
    std::vector<Event> filteredEvents;
    int activeCount = 0;
    int forcesArrivalCount = 0;

    for (const Event& event : events) {
        if (event.get_channel_name() == channelName && event.getEventOwnerUser() == user) {
            filteredEvents.push_back(event);

            // Count stats
            if (event.get_general_information().count("active") &&
                event.get_general_information().at("active") == " true") {
                activeCount++;
            }

            if (event.get_general_information().count("forces_arrival_at_scene") &&
                event.get_general_information().at("forces_arrival_at_scene") == " true") {
                forcesArrivalCount++;
            }
        }
    }
    // Helper function to shorten strings
    auto shortenText = [](const std::string& text, size_t maxLength) {
        if (text.length() > maxLength) {
            return text.substr(0, maxLength - 3) + "...";
        }
        return text;
    };

    // Helper function to convert timestamp to human-readable format
    auto formatDateTime = [](int timestamp) {
        std::time_t time = static_cast<std::time_t>(timestamp);
        char buffer[20];
        if (std::strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", std::localtime(&time))) {
            return std::string(buffer);
        }
        return std::string("Invalid Date");
    };

    // Write header
    outFile << "Channel " << channelName << "\n";
    outFile << "Stats:\n";
    outFile << "Total: " << filteredEvents.size() << "\n";
    outFile << "active: " << activeCount << "\n";
    outFile << "forces arrival at scene: " << forcesArrivalCount << "\n\n";
    outFile << "Event Reports:\n";

    // Write event reports
    int reportNumber = 1;
    for (const Event& event : filteredEvents) {
        outFile << "Report_" << reportNumber++ << ":\n";
        outFile << "  city: " << event.get_city() << "\n";
        outFile << "  date time: " << formatDateTime(event.get_date_time()) << "\n";
        outFile << "  event name: " << event.get_name() << "\n";
        std::string shortenedSummary = shortenText(event.get_description(), 30);
        outFile << "  summary: " << shortenedSummary << "\n";
        outFile << "\n";
    }

    outFile.close();
    std::cout << "Summary successfully written to " << fileName << ".\n";
}