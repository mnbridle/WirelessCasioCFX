#pragma once

#include <Arduino.h>

enum class DatagramType : uint8_t
{
    UNKNOWN = 0x00,
    TEXT_MESSAGE_TX = 0x01,
    TEXT_MESSAGE_RX = 0x02,
    LIST_AVAILABLE_MESSAGES = 0x03,
    MESSAGE_LIST = 0x04,
    MARK_MESSAGE_READ = 0x05,
    IS_MESSAGE_READ = 0x06,
    SET_IDENTITY = 0xF0,
    GET_IDENTITY = 0xF1,
    SET_DATETIME = 0xF2,
    GET_DATETIME = 0xF3,
};
