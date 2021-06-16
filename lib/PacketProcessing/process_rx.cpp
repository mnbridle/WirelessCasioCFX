#include <process_rx.h>
#include <RadioHelpers.hpp>

void main_processor(CFXSerial &cfxSerial) {
    // rename to something more meaningful, prototype code!
    // TODO: Should state machine live inside CFXSerial or here?

    setLEDState(YELLOW);
    // cfxSerial.cfx_software_interface();
    setLEDState(GREEN);

    SystemStatus status;
    VariableData variable_data;
    double led_colour = 0;

    while(1) {
        if(!cfxSerial.execute_current_state())
        {
            delay(100);
        }
        // Get temperature of radio module
        status = getRadioStatus();
        cfxSerial.variable_memory.set('T', status.radio_temperature);
        cfxSerial.variable_memory.set('V', status.batteryVoltage);
        cfxSerial.variable_memory.set('U', status.uptime);

        // Now do something exciting - change the LED colour
        variable_data = cfxSerial.variable_memory.get('L');
        led_colour = variable_data.data.real_part;
        setLEDState((long)led_colour);
    }
}