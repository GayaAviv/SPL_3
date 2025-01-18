#pragma once

#include <string>
#include "Frame.h"

class EncoderDecoder {
public:
    EncoderDecoder();  // Constructor
    ~EncoderDecoder(); // Destructor

    // Encode a Frame object into a string to send to the server
    std::string encode(const Frame& frame) const;

    // Decode a received string into a Frame object
    Frame decode(const std::string& message) const;
};