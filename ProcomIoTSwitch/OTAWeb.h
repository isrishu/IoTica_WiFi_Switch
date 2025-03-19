//#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "otawebconfig.h"
#include "settings.h"

WebServer server(80);
const char* localHost = "esp32";
bool isUpdate = false;

void OTABegin () {
    /*use mdns for host name resolution*/
  if (!MDNS.begin(localHost)) { //http://esp32.local
    Logger.Info("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Logger.Info("mDNS responder started");
  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    char payload[1100];
    snprintf(payload, 1100, loginIndex, SOFTWARE_VER, ADMIN_USERNAME, ADMIN_PASSWORD);
    server.send(200, "text/html", payload);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    Logger.Info("Restarting firmware");
    delay(1000);
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      isUpdate = true;
      Logger.Info("Update: %s", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Logger.Info("Update Success: %u", upload.totalSize);
        Logger.Info("Rebooting...");
      } else {
        Update.printError(Serial);
      }
      isUpdate = false;
    }
  });
  server.begin();
}

void OTAWebHandle () {
  server.handleClient();
  delay(1);
}
