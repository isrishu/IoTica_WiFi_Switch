#ifndef __STRING_PARSER_H__
#define __STRING_PARSER_H__

#include <ArduinoJson.h>
#include "CommandExecuter.h"
#include "CloudAPI.h"

/////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t ValidateJsonFormat(JsonObject& inJson) {
    return (inJson.containsKey(DEVICE_ID) && inJson.containsKey(COMMAND_TO_EXECUTE) && inJson.containsKey(PACKET));
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void processData(String input) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& inJson = jsonBuffer.parseObject(input);
    if(inJson.success()) {
        Logger.InfoPrety(inJson);
        if (inJson.containsKey(ACK)) {
            memset(ack_type, ACK_MAX_LEN, 0);
            String tem = inJson[ACK];
            memcpy(ack_type, tem.c_str(), ACK_MAX_LEN);
        }
        ProcessCommands(inJson);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////

#endif
