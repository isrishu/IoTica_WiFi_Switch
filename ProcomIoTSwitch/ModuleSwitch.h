#ifndef __MODULE_SWITCH_H__
#define __MODULE_SWITCH_H__

#include "powerboard_intrface.h"
#include "procom_global.h"
// #include "mqttSwitch.h"

fanStatusStruct fan_prev_status = {0};

void updateModuleParam(uint8_t module, void *param);

void printSwitchParam(SwitchParams param) {
    Logger.Info("sw_no:%d value:%d", param.sw_no, param.value);
}

SwitchParams* getSwitchPacket(uint8_t *inPacket, uint8_t length) {
    // printArray(__FUNCTION__, inPacket, length);
    static SwitchParams param;
    param.sw_no = inPacket[0];
    param.value = inPacket[1];

    printSwitchParam(param);
	  return &param;
}

void constructMasterONPacket(uint8_t *ret_packet, uint8_t *length) {
    uint8_t i;
    uint8_t index = 0;
    ret_packet[index++] = MODULE_SWITCH;
    ret_packet[index++] = MASTER_SWITCH_NO;
    ret_packet[index++] = pS;
    for(i = 0; i < TOGGLE_SW_NOS; i++) {
        ret_packet[index++] = GET_BIT((*(uint8_t*)&rS), i);
    }

    for(i = 0; i < FAN_NOS; i++) {
        ret_packet[index++] = GET_NIBBLE((*(uint8_t*)&fS), i);
    }

    for(i = 0; i < DIMMER_NOS; i++) {
        ret_packet[index++] = 0;
    }

    *length = index;

    return;
}

void constructNonMasterONPacket(uint8_t *ret_packet, uint8_t sw_no, uint8_t value, uint8_t *length) {
    uint8_t index = 0;
    ret_packet[index++] = MODULE_SWITCH;
    ret_packet[index++] = sw_no;
    ret_packet[index++] = value;
    
    *length = index;
    return;
}

SwitchParams* getSwitchParam(uint8_t sw_no, uint8_t value) {
    static SwitchParams param;
    param.sw_no = sw_no;
    param.value = value;

    printSwitchParam(param);
	return &param;
}

void constructMasterParam() {
    SwitchParams *param = getSwitchParam(0, pS);
    updateModuleParam(MODULE_SWITCH, (void *)param);
    gUpdateCloud = true;
    return;
}

SwitchParams* constructRelayParam(uint8_t sw_no, uint8_t value) {
    return getSwitchParam(sw_no, value);
}

SwitchParams* constructFanParam(uint8_t sw_no, uint8_t value) {
    sw_no += TOGGLE_SW_NOS;
    return getSwitchParam(sw_no, value);
}

SwitchParams* constructDimmerParam(uint8_t sw_no, uint8_t value) {
    sw_no += TOGGLE_SW_NOS + FAN_NOS;
    return getSwitchParam(sw_no, value);
}

void powrboardUpdateRelay()
{
    uint8_t *relayData;
    //rS.relay2=RLY_ON;
    relayData = (uint8_t *)&rS;
    sendRelayData(*relayData);
    //update to MQTT
    // publishToMqtt();
}

void powrboardUpdateFan()
{
    uint8_t *fanData;
    //fS.fan_1=FAN_LOW;
    fanData = (uint8_t *)&fS;
    sendfanData(*fanData);
    // publishToMqtt();
}

void changeRelayStatus(uint8_t relayNo, uint8_t relStatus)
{
    relayNo = relayNo - 1;

    if(relStatus==RLY_TOG) {
        TOGGLE_BIT(*((uint8_t *)&rS), relayNo);
    } else {
        ASSIGN_BIT(*((uint8_t *)&rS), relayNo, relStatus);
    }
    SwitchParams *param = constructRelayParam(relayNo + 1, GET_BIT(*((uint8_t *)&rS), relayNo));
    updateModuleParam(MODULE_SWITCH, (void *)param);
    gUpdateRelay = true;
    gUpdateCloud = true;
    Logger.Info("Relay %d received", relayNo);
    Logger.Info("gUpdateRelay updated");
}

void UpdateStatusValue(uint8_t *addr, uint8_t value) {
    *addr = value;
}

void UpdateToggleSwitchStatus(uint8_t toggleSwitchValue)
{
    UpdateStatusValue((uint8_t *)&rS, toggleSwitchValue);
}

void UpdateFanStatus(uint8_t fanValue)
{
    UpdateStatusValue((uint8_t *)&fS, fanValue);
}

void changeFanStatus(uint8_t fanNo, uint8_t fanOnOFF)
{
    fanNo = fanNo - 1;

    if(fanOnOFF == FAN_CTRL_TOGGLE) //if the command is to toggle the FAN
    {
        if(CMP_NIBBLE(*((uint8_t *)&fS), fanNo, FAN_OFF)) //if current state is OFF, then turn on to previous state
        {
            if(!CMP_NIBBLE(*((uint8_t *)&fan_prev_status), fanNo, FAN_OFF)) 
                fS=fan_prev_status; //Turn on FAN to previous stored mode
            else
            {
                ASSIGN_NIBBLE(*((uint8_t *)&fS), fanNo, FAN_HIGH); //if previous status was OFF then set fan speed to High
            }
        }
        else
        {
            fan_prev_status = fS; //if we are turning off remember the previous state.
            ASSIGN_NIBBLE(*((uint8_t *)&fS), fanNo, FAN_OFF);
        }
    }
    else
    {
        if(fanOnOFF==FAN_OFF)
            fan_prev_status = fS; //store current state
        ASSIGN_NIBBLE(*((uint8_t *)&fS), fanNo, fanOnOFF);
    }

    SwitchParams *param = constructFanParam(fanNo + 1, GET_NIBBLE(*((uint8_t *)&fS), fanNo));
    updateModuleParam(MODULE_SWITCH, (void *)param);
    gUpdateFan = true;
    gUpdateCloud = true;
    Logger.Info("Fan %d received", fanNo);
}

void changeFanMode(uint8_t fanNo, uint8_t fanModectrl)
{
    fanNo = fanNo - 1;

    if((fanModectrl<FAN_CTRL_UP) || (fanModectrl>FAN_CTRL_DOWN)) // fan mode control should be 4/5 nothing else
    {
        Logger.Info("Invalid Fan mode detected..");
        return;
    }

    uint8_t val = GET_NIBBLE(*((uint8_t *)&fS), fanNo);
    Logger.Info("Fan %d mode %d changed %d", fanNo, fanModectrl, val);
    if(fanModectrl == FAN_CTRL_UP) {
        if(val >= FAN_HIGH) {
            val = FAN_HIGH;
        } else {
            val++;
        }
    }
    else // down
    {
        if(val <= FAN_LOW) {
            val = FAN_OFF;
        } else {
            val--;
        }
    }

    ASSIGN_NIBBLE(*((uint8_t *)&fS), fanNo, val);

    SwitchParams *param = constructFanParam(fanNo + 1, GET_NIBBLE(*((uint8_t *)&fS), fanNo));
    updateModuleParam(MODULE_SWITCH, (void *)param);
    gUpdateFan = true;
    gUpdateCloud = true;

    Logger.Info(" --> %d", val);
}

void setFanSpeed(uint8_t fanNo, uint8_t fanSpeed) {
    fanNo = fanNo - 1;
    ASSIGN_NIBBLE(*((uint8_t *)&fS), fanNo, fanSpeed);
    SwitchParams *param = constructFanParam(fanNo + 1, GET_NIBBLE(*((uint8_t *)&fS), fanNo));
    updateModuleParam(MODULE_SWITCH, (void *)param);
    gUpdateFan = true;
    gUpdateCloud = true;
}

void HandleMaster(uint8_t master) {
    Logger.Info("Power value: %d", master);
    if (pS != master) {
        pS = master;
        changeMainPowerStatus(pS);
    }
}

void HandleToggle(uint8_t relayNo, uint8_t value) {
    Logger.Info("Relay %d received", relayNo);
    relayNo = relayNo - 1;
    if (!CMP_BIT(*((uint8_t *)&rS), relayNo, value)) {
        ASSIGN_BIT(*((uint8_t *)&rS), relayNo, value);
        gUpdateRelay = true;
        Logger.Info("gUpdateRelay updated");
    }
}

void HandleDimmer(uint8_t dimmerNo, uint8_t value) {
    // changeDimmerStatus(dimNo, dimmerStatus[dimNo-1]);
}

void HandleFan(uint8_t fanNo, uint8_t value) {
    Logger.Info("Fan %d received", fanNo);
    fanNo = fanNo - 1;
    if (!CMP_NIBBLE(*((uint8_t *)&fS), fanNo, value)) {
        if(value==FAN_OFF) {
            fan_prev_status = fS; //store current state
        }
        ASSIGN_NIBBLE(*((uint8_t *)&fS), fanNo, value);
        gUpdateFan = true;
    }
}

wrk_status SwitchModelHandler(SwitchParams param) {
    uint8_t sw_pos = 0;
    wrk_status status = WRK_SUCCESS;

    if(param.sw_no == 0) {
        HandleMaster(param.value);
        status = WRK_SUCCESS;
    } else if (pS == 0) {
        Logger.Info("Switch %d will not operate while master off", param.sw_no);
        status = WRK_FAILURE;
    } else if (param.sw_no >= MASTER_NOS && param.sw_no < (MASTER_NOS + TOGGLE_SW_NOS)) {
        sw_pos = MASTER_NOS - 1;
        HandleToggle(param.sw_no - sw_pos, param.value);
    } else if (param.sw_no >= (MASTER_NOS + TOGGLE_SW_NOS) && param.sw_no < (MASTER_NOS + TOGGLE_SW_NOS + FAN_NOS)) {
        sw_pos = MASTER_NOS + TOGGLE_SW_NOS - 1;
        HandleFan(param.sw_no - sw_pos, param.value);
    } else if (param.sw_no >= (MASTER_NOS + TOGGLE_SW_NOS + FAN_NOS) && param.sw_no < (MASTER_NOS + TOGGLE_SW_NOS + FAN_NOS + DIMMER_NOS)) {
        sw_pos = MASTER_NOS + TOGGLE_SW_NOS + FAN_NOS - 1;
        HandleDimmer(param.sw_no - sw_pos, param.value);
    } else {
        Logger.Info("Invalid switch number: %d", param.sw_no);
        status = WRK_FAILURE;
    }

    return status;
}

#endif
