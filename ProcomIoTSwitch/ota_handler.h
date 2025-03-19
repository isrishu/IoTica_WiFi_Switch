#ifndef __OTA_HANDLER_H__
#define __OTA_HANDLER_H__

#include <HTTPClient.h>
#include <Update.h>
#include "smart_config.h"

#define HTTP_CODE_401  401
#define HTTP_CODE_SAME_VER 304

boolean checkOtaUpdate=false;
boolean doOtaUpdate=false;
void handleOtaCheck(void);

void handleOtaUpdate()
{
  String overTheAirURL; //="http://cloud.procomhome.com:3081/ota/file?mac=24:0a:c4:00:42:de&version=1.0";

  Logger.Info("Handling OTA check");
  String macAddr = WiFi.macAddress();
  macAddr.toLowerCase();
  //macAddr = "24:0a:c4:00:42:de";
  //mac address should be in small else it will return false
  String http_procom = "http://cloud.procomhome.com:3081/ota/file?mac=";
  String version = cP.version;
  
  overTheAirURL = String(http_procom + macAddr+ "&version=" + version);

  Logger.Info("Firmware update URL: %s", overTheAirURL);

  HTTPClient http2;
  http2.begin(overTheAirURL);

  int httpCode = http2.GET();
  if (httpCode != HTTP_CODE_OK) {
    Logger.Error("HTTP response should be 200");
    return;
  }
  int contentLength = http2.getSize();
  if (contentLength <= 0) {
    Logger.Warning("Content-Length not defined");
    return;
  }

  bool canBegin = Update.begin(contentLength);
  if (!canBegin) {
    Logger.Error("Not enough space to begin OTA");
    return;
  }

  Logger.Info("Update in progress.. please wait..");
  Client& client = http2.getStream();
  int written = Update.writeStream(client);
  if (written != contentLength) {
    Logger.Info("OTA written bytes %d/%d bytes", written, contentLength);
    return;
  }

  if (!Update.end()) {
    Logger.Error("Error #%d", Update.getError());
    return;
  }

  if (!Update.isFinished()) {
    Logger.Info("Update failed.");
    return;
  }
  
  Logger.Info("Update successfully completed. Rebooting.");
  ESP.restart();
  
}

//make sure to call it in loop
void handleOtaCheck()
{
  Logger.Info("Handling OTA check");
  HTTPClient http1;
  String macAddr = WiFi.macAddress();
  macAddr.toLowerCase();
  //macAddr = "24:0a:c4:00:42:de";
  String http_procom = "http://cloud.procomhome.com:3081/ota/status?mac=";
  String version = cP.version;
  
  http_procom = String(http_procom + macAddr+ "&version=" + version);
  Logger.Info(http_procom);
  //http1.begin("http://cloud.procomhome.com:3081/ota/status?mac=24:0A:C4:00:42:DE&version=1.0"); //HTTP
  http1.begin(http_procom); //HTTP
  int httpCode = http1.GET();
  if(httpCode > 0) {
          Logger.Info("[HTTP] GET... code: %d", httpCode); // HTTP header has been send and Server response header has been handled
          if(httpCode == HTTP_CODE_SAME_VER)
          {
            Logger.Info("Same version sono update..");
          }
          else if(httpCode == HTTP_CODE_OK) {
              Logger.Info("Got OTA details");
              String payload = http1.getString();
              //Logger.Info(payload);

              DynamicJsonBuffer jsonBuffer;
              JsonObject& json = jsonBuffer.parseObject(payload);
              Logger.Info(json);
              if (json.success()) {
                if(json["isEnabled"]== "true")
                {
                  //strcpy(cP.otaChecksum, json["checksum"]);
                  String checksumVal=json["checksum"];
                  Logger.Info(checksumVal);
                  doOtaUpdate= true;
                  loop_ota_enable=true;
                }
                else
                {
                  Logger.Info("OTA disabled..");
                }
              }
              //Decode it with json and store the credentials only when it was not matching with old data.
          }
  }
  http1.end();
}

void loop_ota()
{
  if(!loop_ota_enable)
    return;
  if(checkOtaUpdate)
  {
    handleOtaCheck();
    checkOtaUpdate=false;
  }
  if(doOtaUpdate)
  {
    //Run a LED timer to show update is in progress.
    otaLedTicker.attach(otaLedPeriod, otaLedTimerHandle);
    handleOtaUpdate();
    otaLedTicker.detach();
    doOtaUpdate=false;
  }
}

#endif