// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

/*
 * This is an Arduino-based Azure IoT Hub sample for ESPRESSIF ESP32 boards.
 * It uses our Azure Embedded SDK for C to help interact with Azure IoT.
 * For reference, please visit https://github.com/azure/azure-sdk-for-c.
 *
 * To connect and work with Azure IoT Hub you need an MQTT client, connecting, subscribing
 * and publishing to specific topics to use the messaging features of the hub.
 * Our azure-sdk-for-c is an MQTT client support library, helping composing and parsing the
 * MQTT topic names and messages exchanged with the Azure IoT Hub.
 *
 * This sample performs the following tasks:
 * - Synchronize the device clock with a NTP server;
 * - Initialize our "az_iot_hub_client" (struct for data, part of our azure-sdk-for-c);
 * - Initialize the MQTT client (here we use ESPRESSIF's esp_mqtt_client, which also handle the tcp
 * connection and TLS);
 * - Connect the MQTT client (using server-certificate validation, SAS-tokens for client
 * authentication);
 * - Periodically send telemetry data to the Azure IoT Hub.
 *
 * To properly connect to your Azure IoT Hub, please fill the information in the `iot_configs.h`
 * file.
 */

#ifndef __AZURE_H__
#define __AZURE_H__

#include <mqtt_client.h>
#include <string.h>

typedef void (*_azure_callback)(const char * topic, const char * payLoad, unsigned int length);

class Azure {
    private:
        char *deviceKey;
        char *device_id;
        char *host;
        int initializeIoTHubClient();
        int initializeMqttClient();
        // esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event);
        int establishConnection();
    public:
        void setParam(char host[], char device_id[], char key[]);
        void setCallback(_azure_callback callback);
        void initializeTime();
        int azure_setup();
        int azure_listen();
        int azureConnected();
        static _azure_callback az_callback;
};

extern Azure azure;
#endif
