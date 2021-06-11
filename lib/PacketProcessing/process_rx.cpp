#include <process_rx.h>
#include <RadioHelpers.hpp>

void main_processor(CFXSerial &cfxSerial) {
    // rename to something more meaningful, prototype code!
    // TODO: Should state machine live inside CFXSerial or here?

    CFXSerial cfxSerial_inst = cfxSerial;
    RadioStatus status;

    while(1) {
        if(!cfxSerial_inst.execute_current_state())
        {
            delay(100);
        }
        // Get temperature of radio module
        status = getRadioStatus();
        cfxSerial_inst.variable_memory.set('T', status.temperature);
        cfxSerial_inst.variable_memory.set('B', status.batteryVoltage);
        cfxSerial_inst.variable_memory.set('U', millis());
    }
}