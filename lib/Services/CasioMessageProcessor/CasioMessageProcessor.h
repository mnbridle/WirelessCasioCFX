#pragma once

// This is the main application.
#include <CFXSerial.hpp>

#include <Radio/Radio.h>
#include <DeviceSettings/DeviceSettings.h>

#include "codecs/datagrams/datagrams.hpp"

#include <Mailbox/Mailbox.h>

class CasioMessageProcessor {
    public:
        CasioMessageProcessor();
        ~CasioMessageProcessor();
        void run();
        void timer_setup();
    private:
        Radio radio;
        DeviceSettings device_settings;
        CFXSerial cfxSerial;
        Mailbox mailbox;

        bool msg_cfx_to_arm();
        bool msg_arm_to_cfx();

        bool settings_cfx_to_arm(MatrixData);

        void generate_message_list();

        // Set up timers
        unsigned long timer_executecurrentstate;
        unsigned long timer_memoryusage;
        unsigned long timer_sendrfmessage;
        unsigned long timer_getstatus;
        unsigned long timer_debugmodecheck;

        // Set up CFX->ARM message queue


        // Set up buffers
        uint8_t buf[RH_RF24_MAX_MESSAGE_LEN];
};