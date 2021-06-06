#include <process_rx.h>

void main_processor(CFXSerial &cfxSerial) {
    // rename to something more meaningful, prototype code!
    // TODO: Should state machine live inside CFXSerial or here?

    CFXSerial cfxSerial_inst = cfxSerial;

    while(1) {
        if(!cfxSerial_inst.execute_current_state())
        {
            Serial.println("State not transitioned, waiting 1000ms");
            delay(1000);
        }
    }
}