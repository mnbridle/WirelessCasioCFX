#include <CasioMessageProcessor/CasioMessageProcessor.h>

CasioMessageProcessor::CasioMessageProcessor()
{
    cfxSerial = CFXSerial();
    radio = Radio();
    device_settings = DeviceSettings();
    mailbox = Mailbox();

    // Call timer_setup
    timer_setup();

    // Call radio setup
    // RadioSettings_t radio_settings = device_settings.get_radio_settings();
    // drf4463.setFrequency(radio_settings.frequency);
    // drf4463.setFrequency(433.920);
    // drf4463.setModemConfig(RH_RF24::ModemConfigChoice::FSK_Rb0_5Fd1);
    // drf4463.setTxPower(0x05);

    // Initialise cfxSerial serial port
    setUpSerialPort();
    cfxSerial.init();

    // cfxSerial.debugMode = true;

    generate_message_list();
}

CasioMessageProcessor::~CasioMessageProcessor()
{
}

void CasioMessageProcessor::timer_setup()
{
    timer_executecurrentstate = millis();
    timer_memoryusage = millis();
    timer_sendrfmessage = millis();
    timer_getstatus = millis();
    timer_debugmodecheck = millis();
}

bool CasioMessageProcessor::msg_cfx_to_arm()
{
    // Get datagram and convert to buf
    size_t len = 256;
    uint16_t buf[len] = {0};
    size_t data_size = cfxSerial.matrix_memory.size('A');
    cfxSerial.matrix_memory.get_buf('A', buf, len);

    MatrixData raw_datagram = cfxSerial.matrix_memory.get_all('A');
    Serial.println("CFX to ARM - generic request has been received");

    for(size_t i=0; i<data_size; i++)
    {
        Serial.print(i);
        Serial.print(", ");
        Serial.println(buf[i]);
    }

    DatagramType msg_type = static_cast<DatagramType>(buf[0]);
    switch (msg_type)
    {
    case DatagramType::TEXT_MESSAGE_TX:
        // cfxSerial.message_storage.process_sent_message(raw_datagram);
        mailbox.sendMsgToOutbox(buf, data_size);
        break;
    case DatagramType::SET_IDENTITY:
        Serial.println("set_identity message received");
        CasioMessageProcessor::settings_cfx_to_arm(raw_datagram);
        break;
    case DatagramType::GET_IDENTITY:
        Serial.println("get_identity message received");
        break;
    case DatagramType::PROGRAM_TRANSFER:
        Serial.println("Debug mode requested - going into transparent serial mode for 240 seconds");
        cfxSerial.cfx_software_interface(240);
        break;
    case DatagramType::SEND_TO_SERVER:
        Serial.println("Send messages to server");
        break;
    default:
        Serial.println("Unknown message received");
        break;
    }

    return true;
}

bool CasioMessageProcessor::msg_arm_to_cfx()
{
    MatrixData message_to_send_to_cfx;

    Serial.println("Text message has been received");
    message_to_send_to_cfx = cfxSerial.message_storage.process_received_message();
    
    // Ensure that matrix is set to valid
    message_to_send_to_cfx.isValid = true;

    cfxSerial.matrix_memory.init('B', message_to_send_to_cfx.rows, message_to_send_to_cfx.cols, message_to_send_to_cfx.isComplex);
    cfxSerial.matrix_memory.append_matrix('B', message_to_send_to_cfx);
    Serial.println("Done!");
    return true;
}

bool CasioMessageProcessor::settings_cfx_to_arm(MatrixData settings_message)
{
    RadioSettings_t radio_settings;

    /* Schema:
        - MessageType [1,1]
        - Callsign [1,2] - [1,9]
        - Frequency [1,10]
        - Power [1,11]
    */
    
    Serial.println(settings_message.cols);
    
    if (!(settings_message.cols == 11 && settings_message.rows == 1))
    {
        return false;
    }

    radio_settings.frequency = settings_message.matrix_data[9].real_part;
    radio_settings.power = settings_message.matrix_data[10].real_part;
    // radio_settings.data_rate = settings_message.matrix_data[2].real_part;
    radio_settings.data_rate = 500;

    device_settings.log_settings(radio_settings);

    //   drf4463.setFrequency(frequency);
    Serial.print("Frequency set to ");
    Serial.print(radio_settings.frequency);
    Serial.println("MHz");

    return true;
}

void CasioMessageProcessor::run()
{
    bool succeeded = true;

    generate_message_list();

    while(1) {
        if (succeeded || timer_executecurrentstate + 100 < millis())
        {
            succeeded = cfxSerial.execute_current_state();
            timer_executecurrentstate = millis();
        }

        // See if CFX has a text message to send to the ARM
        if (cfxSerial.matrix_memory.is_valid('A') && cfxSerial.matrix_memory.wasReceivedFromCFX('A'))
        {
            Serial.println("CFX sent a text message to ARM");
            msg_cfx_to_arm();
        }

        // Check message queue, to see if there's a message ready to send to the CFX
        if (!cfxSerial.message_storage.inbox_empty())
        {
            Serial.println("ARM has a message to send to CFX");
            msg_arm_to_cfx();
        }
    }
}

void CasioMessageProcessor::generate_message_list()
{
    Message message;

    message.recipient = "MNBRIDLE";
    message.sender = "EMAIL";
    message.date = 24062021;
    message.time = 222755;
    message.message = "Hello world! This is a test message.";
    message.length = 19 + message.message.length();
    cfxSerial.message_storage.send_message_to_inbox(message);

    message.recipient = "MNBRIDLE";
    message.sender = "EMAIL";
    message.date = 25062021;
    message.time = 002630;
    message.message = "Midnight messages are spoooooky, w000000000";
    message.length = 19 + message.message.length();
    cfxSerial.message_storage.send_message_to_inbox(message);

    message.recipient = "MNBRIDLE";
    message.sender = "MNBRIDLE";
    message.date = 25062021;
    message.time = 002730;
    message.message = "It's rather cold and quiet at the moment.";
    message.length = 19 + message.message.length();
    cfxSerial.message_storage.send_message_to_inbox(message);

    message.recipient = "MNBRIDLE";
    message.sender = "EMAIL";
    message.date = 25062021;
    message.time = 002750;
    message.message = "I'm tired & I want to go to bed.";
    message.length = 19 + message.message.length();
    
    cfxSerial.message_storage.send_message_to_inbox(message);
}

// void main_processor(CFXSerial &cfxSerial) {
//     // rename to something more meaningful, prototype code!

//     // Set up receive buffer
//     uint8_t buf[RH_RF24_MAX_MESSAGE_LEN];
//     uint8_t len = sizeof(buf);

//     bool succeeded = true;

//     Serial.println("Configure modem");
//     drf4463.setModemConfig(RH_RF24::ModemConfigChoice::FSK_Rb0_5Fd1);
//     drf4463.setFrequency(432.100);
//     drf4463.setTxPower(0x05);
//     setModeRx();

//     clearLED();

//     while(1) {

//       if (millis() > timer_getstatus + status_sample_interval)
//       {
//         getRadioModuleStatus(cfxSerial);
//         timer_getstatus = millis();
//       }

//       if (succeeded || timer_executecurrentstate + 100 < millis())
//       {
//         succeeded = cfxSerial.execute_current_state();
//         changeLEDColour(cfxSerial);
//         checkForDebugModeRequest(cfxSerial);
//         timer_executecurrentstate = millis();
//       }

//       if (timer_memoryusage + 60000 < millis())
//       {
//         Serial.print("Free memory: ");
//         Serial.println(cfxSerial.freeMemory());
//         timer_memoryusage = millis();
//       }

//       // See if CFX has sent a message to the ARM
//       if (cfxSerial.matrix_memory.is_valid('A') && cfxSerial.matrix_memory.wasReceivedFromCFX('A'))
//       {
//         Serial.println("Datagram received");
//         process_datagram(cfxSerial);
//       }

//       // See if CFX has sent a settings message
//       if (cfxSerial.matrix_memory.is_valid('S') && cfxSerial.matrix_memory.wasReceivedFromCFX('S'))
//       {
//         Serial.println("Settings message received");
//         process_settings_message(cfxSerial);
//       }

//       // See if status sample interval has been updated
//       if (cfxSerial.variable_memory.is_initialised('S'))
//       {
//           VariableData variable_data;

//           // Now do something exciting - change the sample period
//           variable_data = cfxSerial.variable_memory.get('S');
//           status_sample_interval = (unsigned long)variable_data.data.real_part;
//           Serial.print("Changing status sample interval to ");
//           Serial.println(status_sample_interval);
//           cfxSerial.variable_memory.clear('S');
//       }

//       // See if there are any messages to send in Outbox
//       if( !cfxSerial.message_storage.outbox_empty() )
//       {
//           Serial.println("Outbox is not empty!");
//           std::string message = cfxSerial.message_storage.get_serialised_outbox_message();
//           Serial.println("Contents:");
//           Serial.println(message.c_str());
//           setLEDState(RED);

//           drf4463.send((uint8_t *)message.c_str(), strlen(message.c_str()));

//           setLEDState(GREEN);
//       }

//       // Check to see if there's a message to be received
//       if (drf4463.available())
//       {
//         Serial.println("Message available!");
//         // See if there's a message in the buffer
//         while (drf4463.recv(buf, &len))
//         {
//           Serial.println("Message received!");
//           Serial.print("Length: ");
//           Serial.println(len);
//           Serial.print("Message: ");
//           Serial.println((char *)buf);

//           delay(250);
//         }

//         Message received_message;
//         received_message.date = 14012023;
//         received_message.time = 143200;

//         /* RF message schema:
//         recipient = char[8];
//         sender = char[8];
//         date = char[8];
//         time = char[6];
//         message = char[112]
//         */

//         // Iterate through the whole buffer
//         // Recipient field is 8 characters

//         uint8_t i=0;
//         uint8_t j=0;

//         char recipient[9];
//         char sender[9];
//         char date[9];
//         char time[7];
//         char message[113];

//         uint8_t offset = 0;
//         i = 0;
//         while (i < 8) {
//           recipient[i] = buf[i + offset];
//           i++;
//         }
//         recipient[i] = '\0';

//         offset += i;
//         i=0;
//         while (i < 8) {
//           sender[i] = buf[i + offset];
//           i++;
//         }
//         sender[i] = '\0';

//         offset += i;
//         i=0;
//         while (i < 8) {
//           date[i] = buf[i + offset];
//           i++;
//         }
//         sender[i] = '\0';

//         offset += i;
//         i=0;
//         while (i < 6) {
//           time[i] = buf[i + offset];
//           i++;
//         }
//         sender[i] = '\0';

//         offset += i;
//         i=0;
//         while (i < 112 && buf[i + offset] != '\0') {
//           message[i] = buf[i + offset];
//           i++;
//         }
//         message[i] = '\0';

//         received_message.recipient = std::string(recipient);
//         received_message.sender = std::string(sender);
//         received_message.message = std::string(message);
//         received_message.date = atoi(date);
//         received_message.time = atoi(time);
//         received_message.length = 21 + received_message.message.length();

//         cfxSerial.message_storage.send_message_to_inbox(received_message);

//         Serial.print("Final message: ");
//         Serial.println(received_message.recipient.c_str());
//         Serial.println(received_message.sender.c_str());
//         Serial.println(received_message.date);
//         Serial.println(received_message.time);
//         Serial.println(received_message.message.c_str());

//         // Clear memory
//         memset(buf, 0, RH_RF24_MAX_MESSAGE_LEN);
//       }
//     }
// }

// void getRadioModuleStatus(CFXSerial &cfxSerial)
// {
//     SystemStatus status;
//     MatrixData status_data;
//     ComplexValue message_data;

//     // Get temperature of radio module
//     status = getRadioStatus();

//     status_data.cols = 4;
//     status_data.rows = 1;

//     message_data.real_part = status.uptime.real_part;
//     message_data.row = 1;
//     message_data.col = 1;
//     status_data.matrix_data.push_back(message_data);

//     message_data.real_part = status.radio_temperature.real_part;
//     message_data.row = 1;
//     message_data.col = 2;
//     status_data.matrix_data.push_back(message_data);

//     message_data.real_part = status.batteryVoltage.real_part;
//     message_data.row = 1;
//     message_data.col = 3;
//     status_data.matrix_data.push_back(message_data);

//     message_data.real_part = status.lastRssi.real_part;
//     message_data.row = 1;
//     message_data.col = 4;
//     status_data.matrix_data.push_back(message_data);
   
//     status_data.isValid = true;
//     status_data.isComplex = false;
//     status_data.receivedFromCFX = false;

//     cfxSerial.matrix_memory.init('T', status_data.rows, status_data.cols, status_data.isComplex);
//     cfxSerial.matrix_memory.append_matrix('T', status_data);
// }

// void changeLEDColour(CFXSerial &cfxSerial)
// {
//     VariableData variable_data;
//     double led_colour = 0;

//     // Now do something exciting - change the LED colour
//     variable_data = cfxSerial.variable_memory.get('L');
//     led_colour = variable_data.data.real_part;
//     setLEDState((long)led_colour);
// }

