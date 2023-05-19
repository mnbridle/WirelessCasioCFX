#pragma once

#include <string>

struct Message
{
    unsigned int time;
    unsigned int date;
    uint8_t length;
    std::string sender;
    std::string recipient;
    std::string message;
};