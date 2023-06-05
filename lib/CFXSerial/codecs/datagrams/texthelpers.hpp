#pragma once

#include <Arduino.h>
#include <map>

char convert_scancode_to_ascii(uint16_t scancode);
uint16_t convert_ascii_to_scancode(char ascii);
const std::map<char, unsigned short>& ascii_to_scancode(void);
const std::map<unsigned short, char>& scancode_to_ascii(void);