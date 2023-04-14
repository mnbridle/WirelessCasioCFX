#include "Radio.h"

RH_RF24 drf4463(DRF4463_nSEL, DRF4463_nIRQ, DRF4463_SDN);

Radio::Radio() {
    drf4463.setFrequency(433.920);
    drf4463.setModemConfig(RH_RF24::ModemConfigChoice::FSK_Rb0_5Fd1);
    drf4463.setTxPower(0x05);
}

Radio::~Radio() {
}

bool Radio::setup() {
    return drf4463.init();
}

void Radio::setModeRx() {
    drf4463.setModeRx();
}
