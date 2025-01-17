#ifndef FRAME_H
#define FRAME_H

#include <string>
#include <unordered_map>

class Frame {
private:
    std::string command; // The STOMP command (e.g., CONNECT, SEND)
    std::unordered_map<std::string, std::string> headers; // Key-value pairs for headers
    std::string body; // The body of the frame

public:
    // Constructors
    Frame();
    Frame(const std::string& command, const std::unordered_map<std::string, std::string>& headers, const std::string& body);

    // Getters and setters
    const std::string& getCommand() const;
    void setCommand(const std::string& cmd);

    const std::unordered_map<std::string, std::string>& getHeaders() const;
    void addHeader(const std::string& key, const std::string& value);
    void removeHeader(const std::string& key);

    const std::string& getBody() const;
    void setBody(const std::string& bodyContent);

    // Serialize to string
    std::string toString() const;

    // Parse a frame from a string
    static Frame fromString(const std::string& frameString);
};

#endif // FRAME_H
