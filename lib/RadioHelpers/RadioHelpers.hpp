#pragma once

#include <RadioHead.h>
#include <RH_RF24.h>

// Define IO pins
#define DRF4463_SDN 0
#define DRF4463_nIRQ 1
#define DRF4463_SDO 2
#define DRF4463_SCLK 3
#define DRF4463_SDI 4
#define DRF4463_nSEL 13

class RadioStatus
{
    public:
        float batteryVoltage;
        float temperature;
        int16_t lastRssi;
};

bool setupRadio();
void setModeRx();
RadioStatus getRadioStatus();