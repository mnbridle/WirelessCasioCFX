#include <process_rx.h>
#include <RadioHelpers.hpp>

void main_processor(CFXSerial &cfxSerial) {
    // rename to something more meaningful, prototype code!
    // TODO: Should state machine live inside CFXSerial or here?

    while(1) {
        if(!cfxSerial.execute_current_state())
        {
            delay(100);
        }

        getRadioModuleStatus(cfxSerial);
        changeLEDColour(cfxSerial);
        checkForDebugModeRequest(cfxSerial);
    }
}

void getRadioModuleStatus(CFXSerial &cfxSerial)
{
    SystemStatus status;

    // Get temperature of radio module
    status = getRadioStatus();
    cfxSerial.variable_memory.set('T', status.radio_temperature);
    cfxSerial.variable_memory.set('V', status.batteryVoltage);
    cfxSerial.variable_memory.set('U', status.uptime);
}

void changeLEDColour(CFXSerial &cfxSerial)
{
    VariableData variable_data;
    double led_colour = 0;

    // Now do something exciting - change the LED colour
    variable_data = cfxSerial.variable_memory.get('L');
    led_colour = variable_data.data.real_part;
    setLEDState((long)led_colour);
}

void checkForDebugModeRequest(CFXSerial &cfxSerial) {
    // Should we go into debug mode?
    VariableData variable_data;
    unsigned long debug_mode_time = 0;

    variable_data = cfxSerial.variable_memory.get('D');
    debug_mode_time = variable_data.data.real_part;

    if (debug_mode_time >= 1)
    {
        setLEDState(RED);
        cfxSerial.cfx_software_interface(debug_mode_time);
        setLEDState(GREEN);
        cfxSerial.variable_memory.clear('D');
    }
}