#include "ConfigWifi.h"
#include "FreeRTOS.h"
#include "SerialLogger.h"

/*
 * setup function
 */
void setup(void) {
  Logger.SetLogLevel(LOG_NONE);

  //wifi_setup();
  
  freeTROS();
}

void loop(void) {
    String input;
    if (Serial.available()>0) {
        input = Serial.readStringUntil('\n');
        processData(input);
        // StoreDeviceID(input.c_str());
        // ProcessIncomingData(input.c_str());
    }
    
    delay(2000);
}
