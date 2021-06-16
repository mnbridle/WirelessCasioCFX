#include "RadioHelpers.hpp"

RH_RF24 drf4463(DRF4463_nSEL, DRF4463_nIRQ, DRF4463_SDN);


bool setupRadio() {
    return drf4463.init();
}

void setModeRx() {
    drf4463.setModeRx();
}

SystemStatus getRadioStatus() {
  SystemStatus status = SystemStatus();

  status.batteryVoltage.real_part = drf4463.get_battery_voltage();
  status.radio_temperature.real_part = drf4463.get_temperature();
  status.uptime.real_part = millis();
  status.lastRssi.real_part = drf4463.lastRssi();

  return status;
}