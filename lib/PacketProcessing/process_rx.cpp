#include <process_rx.h>
#include <RadioHelpers.hpp>

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

void main_processor(CFXSerial &cfxSerial) {
    // rename to something more meaningful, prototype code!
    unsigned long timer_executecurrentstate = millis();
    unsigned long timer_memoryusage = millis();
    bool succeeded = true;

    while(1) {
        if(succeeded || millis() - timer_executecurrentstate > 100)
        {
            succeeded = cfxSerial.execute_current_state();
            getRadioModuleStatus(cfxSerial);
            changeLEDColour(cfxSerial);
            checkForDebugModeRequest(cfxSerial);
            timer_executecurrentstate = millis();
        }

        if (millis() - timer_memoryusage > 60000)
        {
            Serial.print("Free memory: ");
            Serial.println(freeMemory());
            timer_memoryusage = millis();
        }
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