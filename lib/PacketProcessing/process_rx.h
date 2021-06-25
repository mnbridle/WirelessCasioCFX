#pragma once

#include <CFXSerial.hpp>
#include <RadioHelpers.hpp>
#include <LEDHelpers.hpp>
#include <codecs/CasioCFX/CasioCFX.hpp>

void main_processor(CFXSerial &cfxSerial);
void checkForDebugModeRequest(CFXSerial &cfxSerial);
void changeLEDColour(CFXSerial &cfxSerial);
void getRadioModuleStatus(CFXSerial &cfxSerial);
bool process_datagram(CFXSerial &cfxSerial);
void generate_message_list(CFXSerial &cfxSerial);