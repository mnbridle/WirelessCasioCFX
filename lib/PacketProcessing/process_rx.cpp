#include <process_rx.h>

void main_processor(CFXSerial &cfxSerial) {
    // rename to something more meaningful, prototype code!
    // TODO: Should state machine live inside CFXSerial or here?

    while(1) {
        if(cfxSerial.receivePacket()) {
            Serial.print("Packet received! Packet type: ");
            Serial.print(cfxSerial.packet_type);
            Serial.print(", size: ");
            Serial.println(cfxSerial.size);
        }
        delay(1000);
    }
}