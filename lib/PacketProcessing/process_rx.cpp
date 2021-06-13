#include <process_rx.h>
#include <RadioHelpers.hpp>

void main_processor(CFXSerial &cfxSerial) {
    // rename to something more meaningful, prototype code!
    // TODO: Should state machine live inside CFXSerial or here?

    RadioStatus status;
    double led_colour = 0;

    while(1) {
        if(!cfxSerial.execute_current_state())
        {
            delay(100);
        }
        // Get temperature of radio module
        status = getRadioStatus();
        cfxSerial.variable_memory.set('T', status.temperature);
        cfxSerial.variable_memory.set('V', status.batteryVoltage);
        cfxSerial.variable_memory.set('U', millis());

        // Now do something exciting - change the LED colour
        led_colour = cfxSerial.variable_memory.get('L');
        setLEDState((long)led_colour);
    }
}