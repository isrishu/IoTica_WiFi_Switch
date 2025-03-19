#ifndef __SMART_CONFIG_H__
#define __SMART_CONFIG_H__

#include <WiFi.h>
#include <ArduinoJson.h>
#include "Common.h"
#include "CloudAPI.h"

#define SMART_WIFI_CONNECT_TIMEOUT_S 120
String macAddr;

enum
{
  MQTT_DO_NOTHING,
  HTTP_GET_CRED,
  MQTT_START_CONNECT,
};

// smat config:
enum _SMART_CONFIG
{
   SMART_CONFIG_DO_NOTHING = 0,
   SMART_CONFIG_START,
   SMART_CONFIG_IN_PROGRESS,
   SMART_CONFIG_WIFI_CONN
};

int smartconfig_timeout = 60; //30 Seconds
bool isStaTickerRunning = false;

void IRAM_ATTR connectToWifi() {
  if (strcmp(WIFI_SSID, "") != 0) {
    staLedAttach(TICKER_SLOW, STA_WIFI_CONNECTED);
    Logger.Info("Connecting to Wi-Fi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  } else {
    staLedUpdated = setStaled(STA_NO_OPP_MODE);//update STA LED to Green
    globalStatusLED = STA_NO_OPP_MODE;
    FastLED.show();
  }
}

void WiFiEvent(WiFiEvent_t event) {
    static bool isStaConnUpdated = false;
    switch(event) 
    {
    case SYSTEM_EVENT_STA_GOT_IP:
        Logger.Info("\nWiFi connected");
        Logger.Info("IP address: %s", WiFi.localIP().toString().c_str());
        Logger.Info("Mac address: %s\n", WiFi.macAddress().c_str());
        StaLedTicker.detach();
        isStaTickerRunning = false;
        isStaMqttConnUpdated = false;
        if (sleepStatus == false && isStaConnUpdated == false)
        {
          isStaConnUpdated = true;
          staLedUpdated = setStaled(STA_WIFI_CONNECTED);//update STA LED to Green
          globalStatusLED = STA_WIFI_CONNECTED;
          LED_SLEEP_TIME = 10;
          updateLedsOn();
        }

        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        // isAzureConnected = false;
        isStaConnUpdated = false;
        if (!isConfigMode) {
          Logger.Info("WiFi lost connection");
          Config_SSID = WiFi.SSID();
          Config_pass = WiFi.psk();

          Logger.Info(Config_SSID);
          Logger.Info(Config_pass);

          if (sleepStatus == false && isStaTickerRunning == false)
          {
            isStaTickerRunning = true;
            staLedUpdated = setStaled(STA_WIFI_DISCONNECT);//update STA LED to False
            globalStatusLED = STA_WIFI_DISCONNECT;
            staLedAttach(TICKER_SLOW, STA_WIFI_CONNECTED);
            updateLedsOn();
          }
        }
        break;
      default:
        break;
    }
    if (sleepStatus == true)
    {
      isStaConnUpdated = false;
      isStaTickerRunning = false;
    }
}

void StaLedTimerHandle()
{
   toggleStatusLed();
   staLedUpdated = true;
}

void handleResetDevice()
{
  Logger.println();
  Logger.println();
  Logger.Info("Resetting Device");
  // staLedAttach(TICKER_FAST, STA_RESET_MODE);
  staLedUpdated = setStaled(STA_RESET_MODE);
  globalStatusLED = STA_RESET_MODE;
  FastLED.show();
  bool formatted = SPIFFS.format();
  if (formatted) {
    Logger.Info("Device Successfully Reset");
    Logger.Info("Restarting Device");
    delay(10);
    ESP.restart();
  } else {
    Logger.Error("Error while resetting Device");
  }

  Logger.Info("Resuming Device");
  isConfigMode = false;
  loopCommand = DO_NOTHING;
  // StaLedTicker.detach();
}

int wifi_start_time;
void handleSmartConfigWifi()
{
  static unsigned long Start_time;
  if(wifiCommand == SMART_CONFIG_DO_NOTHING) {
    Start_time =  millis();
    WiFi.disconnect(true);
    if (WiFi.status() == WL_CONNECTED) {
      Logger.Info("Wifi disconnecting.");
    }
    delay(50);
    wifiCommand = SMART_CONFIG_START;
  } else if (wifiCommand == SMART_CONFIG_START) {
    //if not yet disconnected.. wait for disconnect.
    while (WiFi.status() == WL_CONNECTED) {
      delay(500); Serial.println(".");
    }
    Logger.Info(WiFi.status() != WL_CONNECTED ? "DISCONNECTED" : "FAILED");
    
    WiFi.stopSmartConfig();
    delay(10);
    
    WiFi.mode(WIFI_STA);
    boolean statusSmart = WiFi.beginSmartConfig();
    if (!statusSmart) {
      Logger.Info("Failed to initiate smart config....");
      wifiCommand = SMART_CONFIG_DO_NOTHING;
      loopCommand = DO_NOTHING;
      return;
    } else {
      Logger.Info("Waiting for SmartConfig.");
    }

    staLedAttach(TICKER_SLOW, STA_CONFIG_MODE);
    wifiCommand = SMART_CONFIG_IN_PROGRESS;
  } else if (wifiCommand == SMART_CONFIG_IN_PROGRESS) {
    if(WiFi.smartConfigDone()) {
      Logger.Info("Smart config success.. connecting to wifi");
      wifiCommand = SMART_CONFIG_WIFI_CONN;

      staLedAttach(TICKER_SLOW, STA_CONFIG_WiFi_CONNECT_MODE);
      wifi_start_time = millis();
    }
    if((millis() - Start_time) > (smartconfig_timeout * 1000)) {
      Logger.Info("Smart config timeout..");
      StaLedTicker.detach();
      staLedUpdated = true;
      isConfigMode = false;
      WiFi.stopSmartConfig();
      wifiCommand = SMART_CONFIG_DO_NOTHING;
      loopCommand = DO_NOTHING;
      connectToWifi();
    }
  } else if(wifiCommand == SMART_CONFIG_WIFI_CONN) {
     if (WiFi.status() != WL_CONNECTED) {
        Logger.print(".");
        if (millis() - wifi_start_time >= (1000 * SMART_WIFI_CONNECT_TIMEOUT_S)) {
          Logger.Error("\nFailed to connect WiFi. Restarting device....");
          delay(500);
          // StaLedTicker.detach();
          // staLedUpdated = true;
          // isConfigMode = false;
          // wifiCommand = SMART_CONFIG_DO_NOTHING;
          // loopCommand = DO_NOTHING;
          ESP.restart();
        } 
        delay(500);
     } else {
        wrk_status status = WRK_SUCCESS;
        //connected to wifi..
        Config_SSID = WiFi.SSID();
        Config_pass = WiFi.psk();

        Logger.Info(Config_SSID);
        Logger.Info(Config_pass);
        
        StaLedTicker.detach();
        WiFi.stopSmartConfig();
        Logger.Info("Smart config done..");

        macAddr = WiFi.macAddress();
        // macAddr.replace(":", "");
        smart_config_success_callback();
        if (!hasDeviceID) {
          staLedAttach(TICKER_SLOW, STA_CONFIG_REGISTER_MODE);
          // status = startMyServer();
          status = registerDeviceIDByMac(macAddr.c_str());
          staLedDetach();
        }

        isConfigMode = false;
        if (status == WRK_FAILURE || lastStatusLED == STA_MQTT_CONNECTED) {
          staLedUpdated = setStaled(lastStatusLED);
          globalStatusLED = lastStatusLED;
        } 
        else
        {
          staLedUpdated = setStaled(STA_WIFI_CONNECTED);  // make STA LED to Green
          globalStatusLED = STA_WIFI_CONNECTED;
        }
        wifiCommand = SMART_CONFIG_DO_NOTHING;//SMART_CONFIG_DONE;
        loopCommand = DO_NOTHING;
     }
  }

  return;
}

boolean setup_wifi_event()
{
    WiFi.onEvent(WiFiEvent);
    connectToWifi();
    return true;
}

#endif
