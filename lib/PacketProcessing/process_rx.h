#pragma once

#include <CFXSerial.hpp>
//#include <RadioHelpers.hpp>
#include <LEDHelpers.hpp>
#include <codecs/CasioCFX/CasioCFX.hpp>
#include <codecs/datagrams/datagrams.hpp>
#include <RadioHead.h>
#include <RHReliableDatagram.h>
#include <RH_RF24.h>

// Define addresses
#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Define IO pins
#define DRF4463_SDN 0
#define DRF4463_nIRQ 1  // swapped with SDN on Raspberry Pi board
#define DRF4463_SDO 2
#define DRF4463_SCLK 3
#define DRF4463_SDI 4
#define DRF4463_nSEL 13

struct SystemStatus
{
    ComplexValue batteryVoltage;
    ComplexValue uptime;
    ComplexValue radio_temperature;
    ComplexValue lastRssi;
};

bool setupRadio();
void setModeRx();
SystemStatus getRadioStatus();

void main_processor(CFXSerial &cfxSerial);
void checkForDebugModeRequest(CFXSerial &cfxSerial);
void changeLEDColour(CFXSerial &cfxSerial);
void getRadioModuleStatus(CFXSerial &cfxSerial);
bool process_datagram(CFXSerial &cfxSerial);
void generate_message_list(CFXSerial &cfxSerial);