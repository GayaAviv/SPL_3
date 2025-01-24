#pragma once

#include <string>
#include "Frame.h"
#include "../include/StompProtocol.h"
#include "../include/ConnectionHandler.h"


class keyboardInput
{
  public:
     Frame processInput(const std::string& input, StompProtocol& protocol);
     Frame processLogin(const std::string& username,const std::string& password, StompProtocol& protocol);
     Frame processJoin(const std::string& joinInput, StompProtocol& protocol);
     Frame processExit(const std::string& exitInput, StompProtocol& protocol);
     std::vector<Frame> processReport(const std::string& reportInput, StompProtocol& protocol);
     Frame processSummary(const std::string& summaryInput, StompProtocol& protocol);
     Frame processLogout(const std::string& logoutInput, StompProtocol& protocol);
     void writeSummary(const std::string& channelName, const std::string& user, const std::string& fileName, const std::vector<Event>& events);


};