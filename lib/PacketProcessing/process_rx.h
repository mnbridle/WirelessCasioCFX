#pragma once

#include <CFXSerial.hpp>
#include <RadioHelpers.hpp>
#include <LEDHelpers.hpp>
#include <codecs/codec.hpp>

int freeMemory(void);
void main_processor(CFXSerial &cfxSerial);
void checkForDebugModeRequest(CFXSerial &cfxSerial);
void changeLEDColour(CFXSerial &cfxSerial);
void getRadioModuleStatus(CFXSerial &cfxSerial);
