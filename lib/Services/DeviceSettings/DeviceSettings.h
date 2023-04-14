#pragma once

#include <Arduino.h>
#include "definitions/RadioSettings.h"
// #include <FlashStorage_SAMD21.h>

class DeviceSettings {
    public:
        DeviceSettings();
        ~DeviceSettings();

        RadioSettings_t get_radio_settings();
        bool set_radio_settings(RadioSettings_t);
        void log_settings(RadioSettings_t);
};