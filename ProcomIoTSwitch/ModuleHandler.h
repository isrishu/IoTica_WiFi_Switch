#ifndef __MODULE_HANDLER_H__
#define __MODULE_HANDLER_H__

#include <stdlib.h>
#include "Common.h"
#include "module_config.h"
#include "ModuleBell.h"
#include "ModuleSwitch.h"

bool validatePacket(uint8_t packet[], uint8_t length) {
    if (length > 6) {
        return (packet[0] == packet_header[0] && packet[1] == packet_header[1] && packet[length-1] == packet_footer[1] && packet[length-2] == packet_footer[0]);
    }
    return 0;
}

void addHeaderFooter(uint8_t *packet, uint8_t *length) {
    uint8_t ret_packet[OUT_PACKET_ARRAY_MAX];
    memmove(ret_packet, packet, *length);
    uint8_t index = 0;
    packet[index++] = packet_header[0];
    packet[index++] = packet_header[1];
    memmove(&packet[index], &ret_packet[0], *length);
    index += (*length);
    packet[index++] = packet_footer[0];
    packet[index++] = packet_footer[1];
    *length = index;
    // printArray(__FUNCTION__, ret_packet, *length);
    return;
}

bool convertPacketToModule(char *str) {
    uint8_t length = 0;
    uint8_t *inPacket = stringToNumArray (str, &length);

    // printArray(__FUNCTION__, inPacket, length);

    if (!validatePacket(inPacket, length)) {
        Logger.Warning("Invalid packet");
        return 0;
    }

    inModuleParams.module = inPacket[2];
    inModuleParams.param = (void *)getSwitchPacket(&inPacket[3], length - 5);

    return 1;
}

void updateModuleParam(uint8_t module, void *param) {
    outModuleParams.module = module;
    outModuleParams.param = param;
    return;
}

void convertModuleToPacket(uint8_t *packet, uint8_t *length) {
    SwitchParams param;
    switch (outModuleParams.module)
    {
    case MODULE_SWITCH:
        Logger.Info("Switch(%d) Module", outModuleParams.module);
        param = *(SwitchParams*)outModuleParams.param;
        // printSwitchParam(param);
        if (param.sw_no == 0 && param.value == 1) {
            constructMasterONPacket(packet, length);
        } else {
            constructNonMasterONPacket(packet, param.sw_no, param.value, length);
        }
        break;
    
    case MODULE_CURTAIN:
    case MODULE_IR_BLASTER:
    case MODULE_SMART_CUBE:
        Logger.Info("This Module(%d) comming soon", outModuleParams.module);
        break;
    
    default:
        Logger.Info("This Module(%d) not Available", outModuleParams.module);
        break;
    }

	addHeaderFooter(packet, length);
    return;
}

void HandleModule(void* data) {
    wrk_status status = WRK_SUCCESS;
    char* packet = (char*)data;

    // uint8_t packetLen = 0;
    Logger.Info("%s::Packet:%s Len(%d)", __FUNCTION__, packet, strlen(packet));
    if (!convertPacketToModule(packet)){
        return;
    }
    SwitchParams param;
    switch (inModuleParams.module)
    {
    case MODULE_BELL:
        // Logger.Info("BELL(%d) Module", inModuleParams.module);
        triggerCallingBell();
        break;
    
    case MODULE_SWITCH:
        // Logger.Info("Switch(%d) Module", inModuleParams.module);
        param = *(SwitchParams*)inModuleParams.param;
        printSwitchParam(param);
        status = SwitchModelHandler(param);
        break;
    
    case MODULE_CURTAIN:
    case MODULE_IR_BLASTER:
    case MODULE_SMART_CUBE:
        Logger.Info("This Module(%d) comming soon", inModuleParams.module);
        status = WRK_FAILURE;
        break;
    
    default:
        Logger.Info("This Module(%d) not Available", inModuleParams.module);
        status = WRK_FAILURE;
        break;
    }
    // if (status == WRK_SUCCESS) {
    //     gUpdateCloud = true;
    // }
    gUpdateCloud = true;
}

void ProcessOperateBoard(JsonObject& inJson) {
    if (inJson.containsKey(PACKET)) {
        if (strcmp(inJson[PACKET], "") != 0) {
            const char *packet = inJson[PACKET];
            HandleModule((void *)packet);
        }
    }
}
#endif
