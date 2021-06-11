#include "RadioHelpers.hpp"

RH_RF24 drf4463(DRF4463_nSEL, DRF4463_nIRQ, DRF4463_SDN);


bool setupRadio() {
    return drf4463.init();
}

void setModeRx() {
    drf4463.setModeRx();
}

RadioStatus getRadioStatus() {
  RadioStatus status = RadioStatus();

  status.batteryVoltage = drf4463.get_battery_voltage();
  status.temperature = drf4463.get_temperature();
  status.lastRssi = drf4463.lastRssi();

  return status;
}