#include <process_rx.h>

void main_processor(CFXSerial &cfxSerial) {
    // rename to something more meaningful, prototype code!
    while(1) {
        Serial.println("Checking for packet...");
        cfxSerial.receivePacket();
        delay(1000);
    }
}