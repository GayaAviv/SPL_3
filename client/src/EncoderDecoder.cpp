#include "../include/EncoderDecoder.h"
#include <sstream>
#include <iostream>

// Constructor
EncoderDecoder::EncoderDecoder() = default;

// Destructor
EncoderDecoder::~EncoderDecoder() = default;

// Encode a Frame object into a string to send to the server
std::string EncoderDecoder::encode(const Frame& frame) const {
    std::ostringstream encodedFrame;

    // Add the command
    encodedFrame << frame.getCommand() << "\n";

    // Add headers
    for (const auto& header : frame.getHeaders()) {
        encodedFrame << header.first << ":" << header.second << "\n";
    }

    // Add an empty line to separate headers and body
    encodedFrame << "\n";
    
    // Add the body
    encodedFrame << frame.getBody() << '\0'; // Null terminator

    return encodedFrame.str();
}

// Decode a received string into a Frame object
Frame EncoderDecoder::decode(const std::string& message) const {
    std::istringstream receivedFrame(message);
    std::string line;

    // Extract the command
    std::getline(receivedFrame, line);
    std::string command = line;

    // Extract headers
    std::unordered_map<std::string, std::string> headers;
    while (std::getline(receivedFrame, line) && !line.empty()) {
        size_t delimiterPos = line.find(':');
        if (delimiterPos != std::string::npos) {
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);
            headers[key] = value;
        }
    }

    // Extract the body
    std::string body;
    while (std::getline(receivedFrame, line)) {
        body += line + "\n";
    }

    // Remove the last newline character from the body
    if (!body.empty() && body.back() == '\n') {
        body.pop_back();
    }

    return Frame(command, headers, body);
}
