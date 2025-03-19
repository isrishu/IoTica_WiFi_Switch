#ifndef __MODULE_POWER_BOARD_H__
#define __MODULE_POWER_BOARD_H__

#include "procom_global.h"
// #include "mqttSwitch.h"

void powerboardUpdateMainpower()
{
  if(mainpowerStatus == MAIN_PWR_ON)
  {
    // restore to previous saved state? //if not needed 
    // just send the global status to power board
    // gUpdateRelay=true;
    // gUpdateFan=true;
    pS = 1;
    sendpwrData(pS);
    updatePwrLedStatus();
    // publishToMqtt();
  }
  else if(mainpowerStatus == MAIN_PWR_OFF)
  {
    pS = 0;
    sendpwrData(pS);
    //updateAllLedStatus();
    updatePwrLedStatus();
    // publishToMqtt();
  }
  else
  {
    Logger.Info("Mainpower: invalid position reached..");
    #ifdef MQTT_LOG_ENABLE
    logData += "Mainpower: invalid position reached..";
    #endif
    return;
  }
  // publishToMqtt(2,0); //it will publish total data
  constructMasterParam();
  return;
}

void changeMainPowerStatus(uint8_t mainPower)
{
  if(mainPower > 2)
  {
    Logger.Warning("Mainpower: invalid arguments..");
    #ifdef MQTT_LOG_ENABLE
    logData += "Mainpower: invalid arguments..";
    #endif
    return;
  }
  
  if(mainPower == MAIN_PWR_TOGG)
  {
    if(ledOnTickerRunning || ledOffTickerRunning)
    {
      Logger.Info("Key alredy in process, Ignore key Press");
      return;
    }
    else
    {
      mainpowerStatus =! mainpowerStatus;
    }
  }
  else
  {
    if(ledOnTickerRunning || ledOffTickerRunning)
    {
      Logger.Info("Key alredy in process, Ignore key Press");
      return;
    }
    else
    {
      mainpowerStatus = mainPower;
    }
  }

  mainpowerStatusUpdated = true;
}

#endif
