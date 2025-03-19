#include <WiFi.h>
#include "cfg_setting.h"

void wifi_setup() {
  //Just for MAC ID printing
  WiFi.mode(WIFI_MODE_STA);
  Logger.Info("Mac: %s", WiFi.macAddress().c_str());

  setup_config();
 
  // Connect to WiFi network
  if (strcmp(WIFI_SSID, "") != 0) {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Logger.println();

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Logger.print(".");
    }
    Logger.println();
    Logger.Info("Connected to %s", WiFi.SSID().c_str());
    Logger.Info("IP address: %s", WiFi.localIP().toString().c_str());
  }
}
