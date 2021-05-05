#include <RadioHead.h>
#include <RH_RF24.h>
#include "RadioHelpers.hpp"

RH_RF24 drf4463(DRF4463_nSEL, DRF4463_nIRQ, DRF4463_SDN);

bool setupRadio() {
    return drf4463.init();
}

void setModeRx() {
    drf4463.setModeRx();
}

void getRadioStatus(){
  float batteryVoltage = drf4463.get_battery_voltage();
  float temperature = drf4463.get_temperature();
  int16_t lastRssi = drf4463.lastRssi();
  Serial.print("Last RSSI: ");
  Serial.print(lastRssi);
  Serial.println("dBm");
  Serial.print("Battery voltage: ");
  Serial.print(batteryVoltage);
  Serial.print("V, temperature: ");
  Serial.print(temperature);
  Serial.println("'C");
}