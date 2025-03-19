#ifndef __CFG_SETTINGS_H__
#define __CFG_SETTINGS_H__

#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "settings.h"
#include "Common.h"
#include "SerialLogger.h"

enum wrk_status { 
  WRK_FAILURE = 0,
  WRK_SUCCESS
};
struct configParameters
{
  char module[10];
  char model[10];
  char version[10];
  char date[16];
  char wifi_ssid[32];
  char wifi_pwd[20];
  char deviceID[16];
  char sharedAccessKey[64];
  uint32_t led_on_color;
  uint32_t led_off_color;
  int log_level;
};

struct LedRunStatus {
  boolean devRegister;
};

LedRunStatus ledRunStatus = {0};

struct configParameters cP = { //default values
  "SWITCH",
  "8M-10KRM",
  SOFTWARE_VER,
  LAST_UPDATED,
  "",
  "",
  "",
  "",
  0x0000FF,
  0xFF0000,
  (int)LOG_NONE
};

boolean shouldSaveConfig=false;
boolean resetEspPostSave=false;
boolean staLedUpdated = true;
boolean powerkeyIsinTouch = false;
boolean fanDownkeyIsinTouch = false;
boolean isConfigMode = false;
boolean loop_ota_ledstatus = false;  //default enable

// bool isAzureConnected = false;
bool isAzureInitialized = false;

enum CONFIG_MODE { 
  MODE_UNKNOWN = 0,
  RESTART_DEVICE,
  RESET_DEVICE,
  SMART_CONFIG
};
int powerkeyUpdate = MODE_UNKNOWN;

boolean validate_config(JsonObject& json) {
  char *keys[] = {"module", "model", "version", "date", "wifi_ssid", "wifi_pwd", "deviceID", "sharedAccessKey", "led_on_color", "led_off_color", "log_level"};
  int key_number = sizeof(keys)/sizeof(keys[0]);
  for (int i = 0; i < key_number; i++) {
      if(json.containsKey(keys[i])) continue;
      else return 1;
  }
  return 0;
}

boolean setup_config() {
  delay(10);

  //clean FS, for testing
  if(!SPIFFS.begin())
  {
    Logger.msg("Formatting File system");
    SPIFFS.format();
    delay(1000);
  }

  //read configuration from FS json
  Logger.Info("mounting FS...");

  if (SPIFFS.begin()) {
    Logger.Info("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Logger.Info("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Logger.Info("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        if (json.success()) {
          Logger.Info("JSON parsed");
          Logger.Info(json);

          if (validate_config(json) == 1) {
            Logger.msg("Config param in wrong format, Formatting File system");
            SPIFFS.format();
            Logger.msg("File system formated, rebooting....");
            delay(1000);
            ESP.restart();
          }

          strcpy(cP.module, json["module"]);
          strcpy(cP.model, json["model"]);
          strcpy(cP.version, json["version"]);

          strcpy(cP.wifi_ssid, json["wifi_ssid"]);
          if (strcmp(cP.deviceID, "") != 0) {
            hasConfigured = true;
          }
          strcpy(cP.wifi_pwd, json["wifi_pwd"]);
          
          strcpy(cP.deviceID, json["deviceID"]);
          if (strcmp(cP.deviceID, "") != 0) {
            hasDeviceID = true;
          }
          strcpy(cP.sharedAccessKey, json["sharedAccessKey"]);
          
          cP.led_on_color = json["led_on_color"];
          cP.led_off_color = json["led_off_color"];
          int log_level = json["log_level"];
          
          Logger.SetLogLevel(log_level);
          cP.log_level = Logger.GetLogLevel();

          //print all data here
          Logger.Info("module: %s", cP.module);
          Logger.Info("model: %s", cP.model);
          Logger.Info("version: %s", cP.version);
          Logger.Info("date: %s", cP.date);
          Logger.Info("wifi_ssid: %s", cP.wifi_ssid);
          Logger.Info("wifi_pwd: %s", cP.wifi_pwd);
          Logger.Info("deviceID: %s", cP.deviceID);
          Logger.Info("sharedAccessKey: %s", cP.sharedAccessKey);
          Logger.Info("led_on_color: %06X", cP.led_on_color);
          Logger.Info("led_off_color: %06X", cP.led_off_color);
          Logger.Info("log_level: %d", cP.log_level);
          
        } else {
          Logger.msg("Failed to parse json config");
          Logger.msg("Formatting File system");
          SPIFFS.format();
          delay(1000);
          Logger.msg("File system formated, please reboot");
          return false;
        }
        configFile.close();
      }
    }
  } else {
    Logger.msg("failed to mount FS");
    return false;
  }

  Logger.SetLogLevel(cP.log_level);
  //end read
  return true;
}


void smart_config_success_callback()
{
  Logger.Info("connected...yeey :)");
   
  //read updated parameters
  Config_SSID.toCharArray(cP.wifi_ssid , 40);
  Config_pass.toCharArray(cP.wifi_pwd, 20);

  // shouldSaveConfig = true;
}


void save_config()
{
  if (shouldSaveConfig) {
    Logger.Info("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();

    json["module"] = cP.module;
    json["model"] = cP.model;
    json["version"] = cP.version;
    json["date"] = cP.date;
    json["wifi_ssid"] = cP.wifi_ssid;
    json["wifi_pwd"] = cP.wifi_pwd;
    json["deviceID"] = cP.deviceID;
    json["sharedAccessKey"] = cP.sharedAccessKey;
    json["led_on_color"] = cP.led_on_color;
    json["led_off_color"] = cP.led_off_color;
    json["log_level" ] = cP.log_level;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Logger.Error("failed to open config file for writing");
    }

    Logger.Info(json);
    json.printTo(configFile);
    configFile.close();

    shouldSaveConfig = false;

    if(resetEspPostSave)
    {
      delay(10);
      resetEspPostSave=false;
      ESP.restart();
    }

    delay(100);
  }

  return;
}

void saveConfig(JsonObject& json) {
    JSON_VALIDATE_AND_COPY_PARAM(cP.module, json["module"]);
    JSON_VALIDATE_AND_COPY_PARAM(cP.model, json["model"]);
    JSON_VALIDATE_AND_COPY_PARAM(cP.version, json["version"]);
    JSON_VALIDATE_AND_COPY_PARAM(cP.date, json["date"]);
    JSON_VALIDATE_AND_COPY_PARAM(cP.wifi_ssid, json["wifi_ssid"]);
    JSON_VALIDATE_AND_COPY_PARAM(cP.wifi_pwd, json["wifi_pwd"]);
    JSON_VALIDATE_AND_COPY_PARAM(cP.deviceID, json["deviceID"]);
    JSON_VALIDATE_AND_COPY_PARAM(cP.sharedAccessKey, json["sharedAccessKey"]);
    JSON_VALIDATE_AND_COPY_PARAM_INT(cP.led_off_color, json["led_off_color"]);
    JSON_VALIDATE_AND_COPY_PARAM_INT(cP.led_on_color, json["led_on_color"]);
    int log_level = json["log_level"];
    log_level = Logger.SetLogLevel(log_level);
    cP.log_level = (log_level >= 0) ? log_level : cP.log_level ;

    JSON_VALIDATE_AND_RESET_PARAM(json["deviceID"], cP.sharedAccessKey);
    // if (cP.sharedAccessKey[0] == 0) {
    //     isAzureConnected = false;
    // }
    shouldSaveConfig = true;
    // resetEspPostSave = true;
}

#endif
