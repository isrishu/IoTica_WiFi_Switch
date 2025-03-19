#ifndef __AZURE_MAIN_H__
#define __AZURE_MAIN_H__

#include <string.h>
#include "Azure.h"
#include "translator.h"
#include "smart_config.h"
#include "procom_global.h"
#include "SerialLogger.h"

char iotHubHostName[] = "procomiothub.azure-devices.net";

int rectime = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////
/*routine trigger by azzure value change*/
/*MessageCallback*/
static void MessageCallback(const char* topic, const char* payLoad, unsigned int length)
{
  rectime = millis();
  Logger.Info(payLoad);                                                                      /*print the response*/
  
  StaticJsonBuffer<400> jb;
  JsonObject& obj = jb.parseObject(payLoad);
  if (!obj.success()) {
      Logger.Error("Parsing Error 1");
  }

  Logger.Info("Parsing success");
  Logger.Info("deviceID: %s",  obj["deviceID"].as<const char*>());


  String commandToExecute = obj["commandToExecute"];

  //{"commandToExecute":"toggleSwitch", "deviceID":"120141760cfd70bf","packet":"247 3 240 1 0 5 240"}

  if (commandToExecute.compareTo("toggleSwitch") == 0) {                                          /*enter is type is "network"*/
    String temppacket = obj["packet"];
    // int commandLength = 0;
    if (temppacket.compareTo("") == 0) 
    {
      String s_swn = obj["switchNumber"];                                                      /*parse the switch number*/
      String s_per = obj["percent"];                                                      /*parse the percentage*/
      int swn = s_swn.toInt();
      int per = s_per.toInt();
      Logger.Info("switchNumber %s", s_swn);
      Logger.Info("percent %s", s_per);
      Logger.Info("switchNumber %d", swn);
      Logger.Info("percent %d", per);
      toyamaToProcom(swn, per);
    } 
    else 
    {
      // ProcessIncomingData(payLoad);
      processData(payLoad);
    }                                                                      /*send array to sendquery function*/
  }
  Logger.Info("Message callback --> %s", payLoad);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Routine for initialize azure iot hub*/
/*Start of initIoTHub*/
static void initIoTHub()
{
  azure.setParam(iotHubHostName, cP.deviceID, cP.sharedAccessKey);
  azure.setCallback(MessageCallback);
  if (azure.azure_setup() != 0)                      /*initialize Esp32MQTTClient_Init*/
  {
    //    hasIoTHub = false;
    //    Logger.println("Initializing IoT hub failed.");
    Logger.Error("Restarting from azure init..");
    delay(1000);
    ESP.restart();                                                                               /*soft reset if not initialized*/
  }
  isAzureInitialized = true;                                                         /*chang the state once connected*/

  StaLedTicker.detach();
  staLedUpdated = setStaled(STA_MQTT_CONNECTED);//update STA LED to Blue
  globalStatusLED = STA_MQTT_CONNECTED;

  if (sleepStatus == false)
  {
    updateLedsOn();
  }

  constructMasterParam();
}
/*Start of initIoTHub*/

////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
