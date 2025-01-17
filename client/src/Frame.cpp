#include "Frame.h"
#include <sstream>

// Default constructor
Frame::Frame() = default;

// Parameterized constructor
Frame::Frame(const std::string& command, const std::unordered_map<std::string, std::string>& headers, const std::string& body)
    : command(command), headers(headers), body(body) {}

// Getters and setters
const std::string& Frame::getCommand() const {
    return command;
}

void Frame::setCommand(const std::string& cmd) {
    command = cmd;
}

const std::unordered_map<std::string, std::string>& Frame::getHeaders() const {
    return headers;
}

void Frame::addHeader(const std::string& key, const std::string& value) {
    headers[key] = value;
}

void Frame::removeHeader(const std::string& key) {
    headers.erase(key);
}

const std::string& Frame::getBody() const {
    return body;
}

void Frame::setBody(const std::string& bodyContent) {
    body = bodyContent;
}

// Serialize to string
std::string Frame::toString() const {
    std::ostringstream frameStream;
    frameStream << command << "\n";
    for (const auto& header : headers) {
        frameStream << header.first << ":" << header.second << "\n";
    }
    frameStream << "\n" << body << '\0'; // Null character at the end
    return frameStream.str();
}

// Parse a frame from string
Frame Frame::fromString(const std::string& frameString) {
    std::istringstream frameStream(frameString);
    std::string line;
    Frame frame;

    // Parse command
    if (std::getline(frameStream, line)) {
        frame.setCommand(line);
    }

    // Parse headers
    while (std::getline(frameStream, line) && !line.empty()) {
        auto delimiterPos = line.find(':');
        if (delimiterPos != std::string::npos) {
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);
            frame.addHeader(key, value);
        }
    }

    // Parse body
    std::string bodyContent;
    while (std::getline(frameStream, line)) {
        bodyContent += line + "\n";
    }
    if (!bodyContent.empty()) {
        bodyContent.pop_back(); // Remove the last newline
    }
    frame.setBody(bodyContent);

    return frame;
}
