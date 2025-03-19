#ifndef __FREERTOS_H__
#define __FREERTOS_H__

#include "procom_main.h"
#include "UploadTaskHandler.h"
#include "AzureMain.h"
#include "CloudAPI.h"

bool isProcomSetupDone = false;
bool isOTASetupDone = false;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Led otawebupdate task*/
void otawebupdate(void * parameter) {
  int start = millis();
  while (1) {
    if (WiFi.status() == WL_CONNECTED) {
      break;
    }

    if (millis()-start > 10000 && !powerBoradPowerOnStatus) {
      wifi_setup();
      break;
    }
    vTaskDelay(100);
  }

  while (1) {
    if (WiFi.status() == WL_CONNECTED) {
      // OTAWebHandle();
      if (isUpdateAborted && time_last_touch - millis() > 1000) 
      {
        constructMasterParam();
        isUpdateAborted = false;
      }
    }
    vTaskDelay(100);                                /*Task delay inorder to avoid FreeRTOS error*/
  }
  vTaskDelete(NULL);
}
/*end of otawebupdate task*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Led procomTask task*/
void procomTask(void * parameter) {
  while (1) {
    if (!isProcomSetupDone) {
      memcpy(ack_type, ACK_DEVICE, ACK_MAX_LEN);
      setupProcomMain();
      // UDPSetup();
      isProcomSetupDone = true;
    }
    
    loopProcomMain();

    vTaskDelay(50);                                /*Task delay inorder to avoid FreeRTOS error*/
  }
  vTaskDelete(NULL);
}

/*end of procomTask task*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int update_led_status(int status)
{
  //      statusLed(OFF, OFF, OFF, 0, NONE);
  if (status == 1) { /*Call azure interrupt routine*/
    isStaMqttDisConnUpdated = false;
    staLedUpdated = setStaled(STA_MQTT_CONNECTED);//update STA LED to Blue
    globalStatusLED = STA_MQTT_CONNECTED;
    // isAzureConnected = true;
    if (sleepStatus == false && isStaTickerRunning == false && isStaMqttConnUpdated == false)
    {
      isStaMqttConnUpdated = true;
      // staLedDetach();
      updateLedsOn();
    }
  } else if (status == 0)
    {
      isStaMqttConnUpdated = false;
      staLedUpdated = setStaled(STA_MQTT_DISCONNECT);//update STA LED to Green
      globalStatusLED = STA_MQTT_DISCONNECT;
      // isAzureConnected = false;

      if (sleepStatus == false && isStaTickerRunning == false && isStaMqttDisConnUpdated == false)
      {
        isStaMqttDisConnUpdated = true;
        // staLedAttach(TICKER_SLOW, STA_MQTT_CONNECTED);
        updateLedsOn();
      }
    }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*azureTask task*/
void azureTask(void * parameter) {
  while (1) {
    if (isAzureInitialized)
    {  //Enter if internet available and azure iot hub initialized
      int status = azure.azure_listen();

      update_led_status(status);
      if (status != -1)
      {
        publishStatusPayLoad();
      }
    } 
    else
    {
      if (WiFi.status() == WL_CONNECTED && strcmp(cP.deviceID, "") != 0 && !isConfigMode)
      {
        hasDeviceID = true;
        if(cP.sharedAccessKey[0] != 0 && !isAzureInitialized)
        {
            initIoTHub();                                /*Initialize device with azure iothub*/
        }
      }
    }
    vTaskDelay(50);                                /*Task delay inorder to avoid FreeRTOS error*/
  }
  vTaskDelete(NULL);
}
/*end of procomTask task*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Asynchronous task handler using FreeRTOS*/
void freeTROS() {
  /*Task for performing esp interrupts*/
  xTaskCreate(
    otawebupdate,                                  /* Task function. */
    "otawebupdate",                                /* String with name of task. */
    5000,                                         /* Stack size in bytes. */
    NULL,                                          /* Parameter passed as input of the task */
    1,                                             /* Priority of the task. */
    NULL);                                         /* Task handle. */

  /*Task for wifi and internet configuration*/
  xTaskCreate(
    procomTask,                                   /* Task function. */
    "procomTask",                                 /* String with name of task. */
    5000,                                         /* Stack size in bytes. */
    NULL,                                          /* Parameter passed as input of the task */
    1,                                             /* Priority of the task. */
    NULL);                                         /* Task handle. */

  /*Task for wifi and internet configuration*/
  xTaskCreate(
    azureTask,                                   /* Task function. */
    "azureTask",                                 /* String with name of task. */
    10000,                                         /* Stack size in bytes. */
    NULL,                                          /* Parameter passed as input of the task */
    1,                                             /* Priority of the task. */
    NULL);                                         /* Task handle. */
}
/*end of freeRTOS*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
