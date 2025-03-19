#ifndef __COMMAND_EXECUTER_H__
#define __COMMAND_EXECUTER_H__

#include <ArduinoJson.h>
#include "Macros.h"
#include "ModuleHandler.h"

void ProcessGetDeviceID(JsonObject& inJson);
void ProcessSetLogLevel(JsonObject& inJson);
void ProcessSendPacket(JsonObject& inJson);

/////////////////////////////////////////////////////////////////////////////////////////////////
typedef void (*ExecuteCommand)(JsonObject& inJson);
/////////////////////////////////////////////////////////////////////////////////////////////////

// void UDPSendMessage(const char message[]) {
//     const char *outPayload = "{\"deviceId\":\"%s\",\"Message\":%s}";
//     char messagePayload[UDP_MESSAGE_MAX_LEN];
//     snprintf(messagePayload, UDP_MESSAGE_MAX_LEN, outPayload, cP.deviceID, message); /*store switch status in messagePayload*/
//     if (isUDPSetup) {
//         udp.broadcast(messagePayload);
//     }
// }

/////////////////////////////////////////////////////////////////////////////////////////////////


void ProcessCommands(JsonObject& inJson) {
    ExecuteCommand commandExe = NULL;

    if(!inJson.containsKey(COMMAND_TO_EXECUTE)) 
    {
        return;
    }

    if (strcmp(inJson[COMMAND_TO_EXECUTE], "") != 0) 
    {
        const char* command = inJson[COMMAND_TO_EXECUTE];
        Logger.Info("%s\n", command);

        if (strcmp(command, COMMAND_GET_MY_MAC) == 0) 
        {
            String mac = WiFi.macAddress();
            Serial.printf("MAC: %s\n", mac.c_str());
            return;
        }
        
        if (inJson.containsKey(STRING_MAC) || inJson.containsKey(DEVICE_ID)) 
        {
            String mac = WiFi.macAddress();
            String macAddr = inJson[STRING_MAC];
            String deviceId = inJson[DEVICE_ID];
            char *dev_id = (char*)deviceId.c_str();

            if ((mac != macAddr) && strcmp(dev_id, cP.deviceID) != 0) {
                Logger.msg("Invalid user, mac: %s deviceId: %s", macAddr.c_str(), deviceId.c_str());
                return;
            }

            if (strcmp(command, COMMAND_CONFIGURE_DEVICE) == 0) 
            {
                commandExe = saveConfig;
            }
            // else if (strcmp(command, COMMAND_REGISTER_DEVICE) == 0) 
            // {
            //     // commandExe = ProcessRegisterDevice;
            // }
            else if (strcmp(command, COMMAND_GET_DEVICE_ID) == 0) 
            {
                Logger.Info("DeviceID: %s", cP.deviceID);
                // UDPSendMessage("Success");
                return;
            }
            else if (strcmp(command, COMMAND_GET_DEVICE_ID_BY_MAC) == 0) 
            {
                if (WiFi.status() != WL_CONNECTED) {
                    Logger.Info("Not connected to WiFi");
                    return;
                }
                commandExe = ProcessGetDeviceID;
            }
            else if (strcmp(command, COMMAND_SMART_CONFIG) == 0) 
            {
                loopCommand = DO_SMARTCONFIG;
                return;
            }
            else if (strcmp(command, COMMAND_RESET_DEVICE) == 0) 
            {
                loopCommand = DO_RESET;
                staLedDetach();
                return;
            }
            else if (strcmp(command, COMMAND_RESTART_DEVICE) == 0) 
            {
                Logger.Info("Restarting Device....");
                delay(1000);
                ESP.restart();
                return;
            }
            else if (strcmp(command, COMMAND_SET_LOG_LEVEL) == 0) 
            {
                commandExe = ProcessSetLogLevel;
            }
            else if (inJson.containsKey(DEVICE_ID))
            {
                if (strcmp(inJson[DEVICE_ID], cP.deviceID) != 0) {
                    String divID = inJson[DEVICE_ID];
                    Logger.Info("Sorry, Invalid device ID '%s'", divID.c_str());
                    return;
                }
                if (strcmp(command, COMMAND_OTA_VERSION) == 0) 
                {
                    return;
                }
                else if (strcmp(command, COMMAND_OTA_UPDATE) == 0) 
                {
                    return;
                }
                else if (strcmp(command, COMMAND_GET_STATUS) == 0) 
                {
                    return;
                }
                else if ((strcmp(command, COMMAND_OPERATE_BOARD) == 0) || (strcmp(command, COMMAND_TOGGLE_SWITCH) == 0)) 
                {
                    commandExe = ProcessOperateBoard;
                }
                else if (strcmp(command, COMMAND_TOGGLE_SWITCHES) == 0) 
                {
                    commandExe = ProcessSendPacket;
                }
                else 
                {
                    Logger.Info("Sorry, I am not aware of the command '%s'.", command);
                    return;
                }
            }
            else
            {
                Logger.Info("Sorry, you need to provide device ID to execute.");
                return;
            }
        } 
        else 
        {
            Logger.Info("Sorry, you need to provide valid User ID to execute.");
            return;
        }
    }
    else
    {
        Logger.Info("Sorry, you need to provide command to execute.");
        return;
    }

    if (commandExe != NULL) {
        commandExe(inJson);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
#endif
