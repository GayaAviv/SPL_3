#include <string>
#include "Frame.h"

class keyboardInput
{
  private:
     std::string command;

  public:
     Frame processInput(const std::string& input);
     Frame processLogin(const std::string& loginInput);
     Frame processJoin(const std::string& joinInput, StompProtocol protocol);
     Frame processExit(const std::string& loginInput);
     Frame processReport(const std::string& loginInput);
     Frame processSummary(const std::string& loginInput);
     Frame processLogout(const std::string& loginInput);


};