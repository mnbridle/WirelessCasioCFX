#include "DeviceSettings.h"

// Store and retrieve non-volatile data, such as radio settings.

// FlashStorage(radio_settings_mem, RadioSettings_t);

// init and deinit methods
DeviceSettings::DeviceSettings() {
}

DeviceSettings::~DeviceSettings() {
}

bool DeviceSettings::set_radio_settings(RadioSettings_t radio_settings) {
    // Pass in radio_settings - return success state
    Serial.println("Attempting to save radio settings is not implemented yet");
    log_settings(radio_settings);

    // Check to see if data has already been saved
    // RadioSettings_t current_settings = radio_settings_mem.read();
    // if (!current_settings.valid)
    // {
    //     Serial.println("Nothing saved in memory!");
    // }
    
    // return current_settings.valid;

    return false;
}

RadioSettings_t DeviceSettings::get_radio_settings() {
    // RadioSettings_t current_settings = radio_settings_mem.read();
    RadioSettings_t current_settings;

    return current_settings;
}

void DeviceSettings::log_settings(RadioSettings_t settings) {
    // Log radio settings
    Serial.println("Settings data - contents:");
    Serial.print("Callsign: ");
    Serial.println(settings.callsign);
    Serial.print("Frequency: ");
    Serial.println(settings.frequency);
    Serial.print("Data rate: ");
    Serial.println(settings.data_rate);
    Serial.print("Power: ");
    Serial.println(settings.power);
}

