#include <process_rx.h>
#include <RadioHelpers.hpp>

void main_processor(CFXSerial &cfxSerial) {
    // rename to something more meaningful, prototype code!
    unsigned long timer_executecurrentstate = millis();
    unsigned long timer_memoryusage = millis();
    unsigned long timer_receivemessage_test = millis();
    bool succeeded = true;

    // Force message into queue
    generate_message_list(cfxSerial);

    timer_receivemessage_test = millis();

    while(1) {
        if (succeeded || millis() - timer_executecurrentstate > 100)
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
          Serial.println(cfxSerial.freeMemory());
          timer_memoryusage = millis();
        }

        // See if CFX has sent a message to the ARM
        if (cfxSerial.matrix_memory.is_valid('A') && cfxSerial.matrix_memory.wasReceivedFromCFX('A'))
        {
          Serial.println("Datagram received");
          process_datagram(cfxSerial);
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

void checkForDebugModeRequest(CFXSerial &cfxSerial) 
{
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

bool process_datagram(CFXSerial &cfxSerial)
{
  // Do something with the message!
  MatrixData raw_datagram = cfxSerial.matrix_memory.get_all('A');
  DatagramType datagram_type = cfxSerial.message_storage.message_type(raw_datagram);

  MatrixData message_to_send;

  switch(datagram_type)
  {
    case DatagramType::TEXT_MESSAGE_TX :
      Serial.println("Received text message to on-send, processing it");
      cfxSerial.message_storage.process_sent_message(raw_datagram);
      break;

    case DatagramType::TEXT_MESSAGE_RX :
      Serial.println("Text message requested!");
      message_to_send = cfxSerial.message_storage.process_received_message();
      Serial.print("Message generated.");
      cfxSerial.matrix_memory.append_matrix('A', message_to_send);
      Serial.println("Sent to matrix memory to be retrieved");
      break;

    case DatagramType::UNKNOWN :
    default:
      Serial.print("Unknown datagram received! Rejecting.");
  }

  return true;
}

void generate_message_list(CFXSerial &cfxSerial)
{
    Message message;

    message.recipient = "MNBRIDLE";
    message.sender = "EMAIL";
    message.date = 24062021;
    message.time = 222755;
    message.message = "Hello world! This is a test message.";
    message.length = 19 + message.message.length();
    cfxSerial.message_storage.send_message_to_receive_queue(message);

    message.recipient = "MNBRIDLE";
    message.sender = "EMAIL";
    message.date = 25062021;
    message.time = 002630;
    message.message = "Midnight messages are spoooooky, w000000000";
    message.length = 19 + message.message.length();
    cfxSerial.message_storage.send_message_to_receive_queue(message);

    message.recipient = "MNBRIDLE";
    message.sender = "MNBRIDLE";
    message.date = 25062021;
    message.time = 002730;
    message.message = "It's rather cold and quiet at the moment.";
    message.length = 19 + message.message.length();
    cfxSerial.message_storage.send_message_to_receive_queue(message);

    message.recipient = "MNBRIDLE";
    message.sender = "EMAIL";
    message.date = 25062021;
    message.time = 002750;
    message.message = "I'm tired & I want to go to bed.";
    message.length = 19 + message.message.length();
    cfxSerial.message_storage.send_message_to_receive_queue(message);
}