#pragma once

#include <RadioHead.h>
#include <RH_RF24.h>
#include <codecs/CasioCFX/CasioCFX.hpp>

// Define IO pins
#define DRF4463_SDN 0
#define DRF4463_nIRQ 1
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