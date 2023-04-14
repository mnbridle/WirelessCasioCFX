#pragma once

#include <RadioHead.h>
#include <RHReliableDatagram.h>
#include <RH_RF24.h>

// Define IO pins
#define DRF4463_SDN 0
#define DRF4463_nIRQ 1  // swapped with SDN on Raspberry Pi board
#define DRF4463_SDO 2
#define DRF4463_SCLK 3
#define DRF4463_SDI 4
#define DRF4463_nSEL 13

class Radio {
    public:
        Radio();
        ~Radio();
        bool setup();
        void setModeRx();
};