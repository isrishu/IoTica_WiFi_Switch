/*
 * smartSwitchBoard.ino
 * Author: Venugopal reddy somu
 * Copyright @ Bridgethings pvt ltd.
 * All rights are reserved.
 * 
 * 
 */

#ifndef __PROCOM_MAIN_H__
#define __PROCOM_MAIN_H__

#include <StreamString.h>   // Webserver, Updater
#include <Wire.h> 
#include <Ticker.h>
#include <ESP32Ping.h>
#include "cfg_setting.h"
#include "powerboard_intrface.h"
#include "procom_global.h"
#include "led_interface.h"
// #include "mqttSwitch.h"
#include "smart_config.h"
#include "irReceiver.h"
#include "bt_touch_interface.h"
#include "StringParser.h"
#include "ModulePowerBoard.h"

boolean setup_wifi_event();

void ChangeFanStatus(uint8_t fanNo, uint8_t fanOnOFF)
{
  switch(fanNo)
  {
    case 1:
           if(fanOnOFF == FAN_CTRL_TOGGLE) //if the command is to toggle the FAN
           {
               if(fS.fan_1==FAN_OFF) //if current state is OFF, then turn on to previous state
               {
                  if(fan1_prev_status!=FAN_OFF) 
                     fS.fan_1=fan1_prev_status; //Turn on FAN to previous stored mode
                  else
                  {
                     fS.fan_1=FAN_HIGH;  //if previous status was OFF then set fan speed to High
                     //fan1_prev_status=fS.fan_1;
                  }
               }
               else
               {
                   fan1_prev_status = fS.fan_1; //if we are turning off remember the previous state.
                   fS.fan_1=FAN_OFF;
               }
           }
           else
           {
              if(fanOnOFF==FAN_OFF)
                 fan1_prev_status = fS.fan_1; //store current state
              fS.fan_1 = fanOnOFF;
           }
           break;
    case 2:
           if(fanOnOFF == FAN_CTRL_TOGGLE) //if the command is to toggle the FAN
           {
               if(fS.fan_2==FAN_OFF) //if current state is OFF, then turn on to previous state
               {
                  if(fan2_prev_status!=FAN_OFF) 
                     fS.fan_2=fan2_prev_status; //Turn on FAN to previous stored mode
                  else
                  {
                     fS.fan_2=FAN_HIGH;  //if previous status was OFF then set fan speed to High
                     //fan2_prev_status=fS.fan_2;
                  }
               }
               else
               {
                   fan2_prev_status = fS.fan_2; //if we are turning off remember the previous state.
                   fS.fan_2=FAN_OFF;
               }
           }
           else
           {
              if(fanOnOFF==FAN_OFF)
                 fan2_prev_status = fS.fan_2; //store current state
              fS.fan_2 = fanOnOFF;
           }
           break;
    default:
           break;
  }
  gUpdateFan = true;
}

void setupProcomMain() {
   // put your setup code here, to run once:
   //   boolean status;

   Logger.println();

   //initialise I2C
   Wire.begin(I2C_SDA,I2C_SCL);

   delay(10);
   //initialise Atmega power board, and reads the stored EEPROM data to global variables.
   powerboard_begin();

   #ifdef MQTT_LOG_ENABLE
   logData = logData +"\n" + "Power ON Status:";
   #endif
   Logger.Info("Power ON Status: %d", grecRelayData);

   //Calling bell should always to be off during power reset/bootup.
   #ifdef TOUCH_2_MODULE_BELL
   rS.relay1=0;
   #endif
   
   rS = *((struct relayStatusStruct *) &grecRelayData);
   fS = *((struct fanStatusStruct *) &grecFanData);
   pS = grecPwrData;
   
   setup_led();

   #ifndef TOUCH_8_MODULE
   pS =1;
   #endif
   //set relay's &LED's to previous ON Status
   sendpwrData(pS);
   updatePonLedStatus();

   setup_config();
   
   //init touch IC's -> call and make sure both IC's are working.
   setup_touch();
   setup_IrRecever();  //init ir
   delay(2000);
   
   //Just for MAC ID printing
   //   WiFi.mode(WIFI_MODE_STA);
   //   delay(2000);
   //   Logger.Info("Mac: %s", WiFi.macAddress().c_str());
   
   setup_wifi_event();  //init WIFI and Mqtt
   LED_SLEEP_TIME = 20;

   delay(1);
}

void loopProcomMain()
{
   // if there is any update to power board inform via I2C:

   {
      if(gUpdateRelay)
      {
         Logger.Info("sending relay data");
         #ifdef MQTT_LOG_ENABLE
         logData = logData +"\n" + "Sending relay data";
         #endif
         powrboardUpdateRelay();
         updateAllLedStatus();
         gUpdateRelay =false;
      }
      
      if(gUpdateFan)
      {
         Logger.Info("sending Fan data");
         #ifdef MQTT_LOG_ENABLE
         logData = logData +"\n" + "sending Fan data";
         #endif
         powrboardUpdateFan();
         updateAllLedStatus();
         gUpdateFan = false;
      }
      
      if(mainpowerStatusUpdated)
      {
         Logger.Info("sending Main power data");
         #ifdef MQTT_LOG_ENABLE
         logData = logData +"\n" + "sending Main power data";
         #endif
         powerboardUpdateMainpower();
         mainpowerStatusUpdated =false;
      }

      // loop_wifi_mqtt();
      //handlesmartConfig only when required.
      if(loopCommand == DO_SMARTCONFIG)
      {
         handleSmartConfigWifi();
         ledSleepEnable();
      }
      else if (loopCommand == DO_RESET)
      {
         handleResetDevice();
         ledSleepEnable();
      }
      else if (loopCommand == DO_OTA_UPDATE)
      {
         isConfigMode = false;
      }
   
      loop_IrReceiver();
      loop_touch();
      save_config();
   }

   loop_led();
}

#endif