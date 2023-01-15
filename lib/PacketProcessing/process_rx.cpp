#include <process_rx.h>

RH_RF24 drf4463(DRF4463_nSEL, DRF4463_nIRQ, DRF4463_SDN);

bool setupRadio() {
    return drf4463.init();
}

void setModeRx() {
    drf4463.setModeRx();
}

SystemStatus getRadioStatus() {
  SystemStatus status = SystemStatus();

  status.batteryVoltage.real_part = drf4463.get_battery_voltage();
  status.radio_temperature.real_part = drf4463.get_temperature();
  status.uptime.real_part = millis();
  status.lastRssi.real_part = drf4463.lastRssi();

  return status;
}

void main_processor(CFXSerial &cfxSerial) {
    // rename to something more meaningful, prototype code!
    unsigned long timer_executecurrentstate = millis();
    unsigned long timer_memoryusage = millis();
    unsigned long timer_sendrfmessage = millis();

    // Set up receive buffer
    uint8_t buf[RH_RF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    bool succeeded = true;

    Serial.println("Configure modem");
    drf4463.setModemConfig(RH_RF24::ModemConfigChoice::FSK_Rb0_5Fd1);
    drf4463.setFrequency(433.920);
    drf4463.setTxPower(0x05);

    while(1) {
      clearLED();
      if (succeeded || timer_executecurrentstate + 100 < millis())
      {
        succeeded = cfxSerial.execute_current_state();
        getRadioModuleStatus(cfxSerial);
        changeLEDColour(cfxSerial);
        checkForDebugModeRequest(cfxSerial);
        timer_executecurrentstate = millis();
      }

      if (timer_memoryusage + 60000 < millis())
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

      // See if there are any messages to send in Outbox
      if( !cfxSerial.message_storage.outbox_empty() )
      {
          Serial.println("Outbox is not empty!");
          std::string message = cfxSerial.message_storage.get_serialised_outbox_message();
          Serial.println("Contents:");
          Serial.println(message.c_str());
          setLEDState(RED);

          drf4463.send((uint8_t *)message.c_str(), strlen(message.c_str()));

          setLEDState(GREEN);
      }

      // Check to see if there's a message to be received
      if (drf4463.available())
      {
        delay(2000);
        // See if there's a message in the buffer
        if (drf4463.recv(buf, &len))
        {
          Serial.println("Message received!");
          Serial.print("Length: ");
          Serial.println(len);
          Serial.print("Message: ");
          Serial.println((char *)buf);

          Message received_message;
          received_message.date = 14012023;
          received_message.time = 143200;

          // Iterate through the whole buffer
          // Recipient field is 8 characters

          uint8_t i=0;
          uint8_t j=0;

          char recipient[9];
          char sender[9];
          char message[113];

          while (i < 8) {
            recipient[j] = buf[i];
            i++;
            j++;
          }

          recipient[j] = '\0';
          j=0;

          while (i < 16) {
            sender[j] = buf[i];
            i++;
            j++;
          }

          sender[j] = '\0';
          j=0;

          while (i < 128 && buf[i] != '\0') {
            message[j] = buf[i];
            i++;
            j++;
          }

          message[j] = '\0';
          j=0;

          received_message.recipient = std::string(recipient);
          received_message.sender = std::string(sender);
          received_message.message = std::string(message);
          received_message.length = 19 + received_message.message.length();

          cfxSerial.message_storage.send_message_to_inbox(received_message);

          // Clear memory
          memset(buf, 0, RH_RF24_MAX_MESSAGE_LEN);
        }
        else
        {
          Serial.println("Recv failed?");
        }
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
  MatrixData message_to_send_to_cfx;

  // Do something with the message!
  MatrixData raw_datagram = cfxSerial.matrix_memory.get_all('A');
  DatagramType datagram_type = cfxSerial.message_storage.message_type(raw_datagram);

  switch(datagram_type)
  {
    case DatagramType::TEXT_MESSAGE_TX :
      Serial.println("Text message has been sent");
      cfxSerial.message_storage.process_sent_message(raw_datagram);
      Serial.println("Done!");
      return true;

    case DatagramType::UNKNOWN :
    default:
      Serial.print("Unknown datagram received! Rejecting.");
  }

  // Message in inbox?
  if (!cfxSerial.message_storage.inbox_empty())
  {
      Serial.println("Text message has been received");
      message_to_send_to_cfx = cfxSerial.message_storage.process_received_message();
      
      // Ensure that matrix is set to valid
      message_to_send_to_cfx.isValid = true;

      cfxSerial.matrix_memory.init('B', message_to_send_to_cfx.rows, message_to_send_to_cfx.cols, message_to_send_to_cfx.isComplex);
      cfxSerial.matrix_memory.append_matrix('B', message_to_send_to_cfx);
      Serial.println("Done!");
      return true;
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
    cfxSerial.message_storage.send_message_to_outbox(message);

    message.recipient = "MNBRIDLE";
    message.sender = "EMAIL";
    message.date = 25062021;
    message.time = 002630;
    message.message = "Midnight messages are spoooooky, w000000000";
    message.length = 19 + message.message.length();
    cfxSerial.message_storage.send_message_to_outbox(message);

    message.recipient = "MNBRIDLE";
    message.sender = "MNBRIDLE";
    message.date = 25062021;
    message.time = 002730;
    message.message = "It's rather cold and quiet at the moment.";
    message.length = 19 + message.message.length();
    cfxSerial.message_storage.send_message_to_outbox(message);

    message.recipient = "MNBRIDLE";
    message.sender = "EMAIL";
    message.date = 25062021;
    message.time = 002750;
    message.message = "I'm tired & I want to go to bed.";
    message.length = 19 + message.message.length();
    
    cfxSerial.message_storage.send_message_to_outbox(message);
}