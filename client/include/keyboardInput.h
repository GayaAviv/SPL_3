#include <string>
#include "Frame.h"

class keyboardInput
{
  private:
     std::string command;

  public:
     Frame processInput(const std::string& input, StompProtocol protocol);
     Frame processLogin(const std::string& loginInput);
     Frame processJoin(const std::string& joinInput, StompProtocol protocol);
     Frame processExit(const std::string& exitInput, StompProtocol protocol);
     Frame processReport(const std::string& reportInput, StompProtocol protocol);
     Frame processSummary(const std::string& summaryInput, StompProtocol protocol);
     Frame processLogout(const std::string& logoutInput, StompProtocol protocol);


};