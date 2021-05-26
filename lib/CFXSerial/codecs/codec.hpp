#include <Arduino.h>

struct VariableRequestPacket {
    const char startbyte = ':';
    const char packet_type[3] = {'R', 'E', 'Q'};
    const char padding0 = '\0';
    char variable_type[2];
    const char padding1[4] = {'\xFF'};
    char variable_name;
    const char padding2[47] = {'\xFF'};
    uint8_t checksum;
};

struct DataWakeup {
    const char startbyte = '\x15';
};

struct ScreenshotWakeup {
    const char startbyte = '\x16';
};
