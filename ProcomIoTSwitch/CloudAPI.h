#ifndef __CLOUD_API_H__
#define __CLOUD_API_H__

// #include <WiFi.h>
// #include <HTTPClient.h>
#include <ArduinoJson.h>
#include "procom_global.h"
#include "AsyncHttpPost.h"
#include "UploadTaskHandler.h"

#define REGISTER_TASK_NO 0

int registerCount = 0;
wrk_status regStatus = WRK_FAILURE;

// const char *token = "Bearer Tk,qD)Mgg,E3J!.DcxHLEXBcU;Fy4#HKM)njQ&y=(6F_z]qMDuwi7hdhTuEZUw24";

typedef wrk_status (*fnProcessRespMessage)(char ch[], size_t len);

wrk_status parseAccessKeyFromMessage(char ch[], size_t len) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& inJson = jsonBuffer.parseObject(ch);
    if (inJson.success()) {

        
    } else {
        Logger.Error("Error in parsing\n");
    }
    return WRK_FAILURE;
}

wrk_status parseDeviceConfigFromMessage(char ch[], size_t len)
{
    DynamicJsonBuffer jsonBuffer;
    JsonObject& inJson = jsonBuffer.parseObject(ch);
    if (inJson.success()) {
        String deviceID = inJson["Message"]["deviceID"];
        if (deviceID != "") {
            Logger.Info("JSON parsing is success, deviceID: %s\n", deviceID.c_str());
            // Logger.InfoPrety(inJson);
            JSON_VALIDATE_AND_COPY_PARAM(cP.deviceID, inJson["Message"]["deviceID"]);
            hasDeviceID = true;

            String sharedKey = inJson["Message"]["deviceKey"];
            if (sharedKey != "") {
                Logger.Info("JSON parsing is success, sharedAccessKey: %s\n", sharedKey.c_str());
                // Logger.InfoPrety(inJson);
                JSON_VALIDATE_AND_COPY_PARAM(cP.sharedAccessKey, inJson["Message"]["deviceKey"]);
                Logger.Info("Device Registered successfully\n");
                finishTask((void *)&gTaskList[REGISTER_TASK_NO]);
                return WRK_SUCCESS;
            }
        }
    } else {
        Logger.Error("Error in parsing\n");
    }

    abortTask((void* )&gTaskList[REGISTER_TASK_NO]);
    return WRK_FAILURE;
}

wrk_status parseResponseFromCloud(void *args, void* data, size_t len) {
    char * p1, *p2;
    char * ch = (char*)data;
    ch[len] = '\0';
    Logger.Info("Response --> %s", ch);
    wrk_status ret = WRK_FAILURE;
    fnProcessRespMessage processDeviceConfig = (fnProcessRespMessage) args;
    
    p1 = strstr (ch, "{\"Status\":\"Success\"");                     /* check for success response (return address of string occurrence) */
    p2 = strstr (ch, "\"Status\": \"Success\"");
    int position = p1 - ch;
    Logger.Info("Position: %d, p1: %d, p2: %d, len: %d", position, (int)p1, (int)p2, len);
    if (position >= 0) {
        size_t st_len = len - position;
        char st[st_len];
        strncpy(st, p1, st_len);
        st[st_len] = '\0';
        Logger.Info("Response Body --> %s", st);
        if (p1 || p2) {
           Logger.Info("Success...");
           ret = processDeviceConfig(st, st_len);
        }
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void parseDataFromHttpPostResponseCommon(void *args, void* data, size_t len) {
    if (!args) return;

    if (parseResponseFromCloud((void*)parseDeviceConfigFromMessage, data, len) != WRK_SUCCESS) 
    {
        Logger.Warning("Error on sending POST");
    }
    return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////

wrk_status tryToRegisterDeviceIDByMac (const char* mac) {
    const char *payload = "{\"macID\":\"%s\",\"version\":\"%s\"}";
    char messagePayload[MESSAGE_MAX_LEN];
    wrk_status ret = WRK_FAILURE;

    snprintf(messagePayload, MESSAGE_MAX_LEN, payload, mac, SOFTWARE_VER);
    Logger.Info("GetDeviceIDByMAC: %s", messagePayload);

    uint8_t taskNo = REGISTER_TASK_NO;
    isAsycTCPConfigured = false;
    createContext(taskNo, messagePayload);
    gTaskList[taskNo].reqParams.callback = parseDataFromHttpPostResponseCommon;
    gTaskList[taskNo].reqParams.api = registerDeviceApi;
    gTaskList[taskNo].reqParams.host = procom_azure_url;

    Logger.Info("**** Task %d added ****", gTaskList[taskNo].taskNo);

    sendHttpsPostRequest((void *)&gTaskList[taskNo]);
    Logger.Info("Wait for Registration..");
    while (gTaskList[taskNo].taskStatus < TASK_DONE) delay(100);
    if (gTaskList[taskNo].taskStatus)
    {
        ret = WRK_SUCCESS;
    }

    isAsycTCPConfigured = false;
    Logger.Info("Saving configuration..");
    {
        // Small delay for show registration status
        int i = 0;
        while (i++ < 50) {Logger.print("."); delay(100);}
        Logger.print("\n");
    }

    return ret;
}

wrk_status registerDeviceIDByMac (const char* mac) {
    wrk_status status;
    // while(1) 
    {
        long timeNow = millis();
        status = tryToRegisterDeviceIDByMac(mac);
        Logger.Info("Response time --> %d ms", (int)(millis() - timeNow));
        if (status == WRK_SUCCESS) {
            retry_count = 0;
            Logger.Info(F("Registration Successfull"));
            shouldSaveConfig = true;
            save_config();
            return status;
        } 
        // else {
        //     if (++retry_count > MAX_RETRY) {
        //         break;
        //     }
        //     Logger.Info("********************\nStatus Register, Retry %d/%d!\n********************\n", retry_count, MAX_RETRY);
        // }
    }

    // Logger.Info("********************\nStatus Register, Retry Aborted!\n********************\n");
    retry_count = 0;
    gUpdateCloud = false;
    return status;
}

void ProcessGetDeviceID(JsonObject& inJson)
{
    String macAddr = inJson["macID"];
    if (macAddr == "") {
        macAddr = WiFi.macAddress();
    }
    if (registerDeviceIDByMac(macAddr.c_str())) {
        staLedUpdated = setStaled(STA_WIFI_CONNECTED);  // make STA LED to Green
        globalStatusLED = STA_WIFI_CONNECTED;
    }
}

void ProcessSetLogLevel(JsonObject& inJson) {
    String mac = WiFi.macAddress();
    String macAddr = inJson["macID"];
    if (macAddr == mac) {
        int log_level = inJson["log_level"];
        // Logger.SetLogLevel(log_level);
        // cP.log_level = Logger.GetLogLevel();
        saveConfig(inJson);
        // save_config();
        Logger.msg("Logger level: %d", log_level);
    } else {
        Logger.msg("Invalid user: %s", macAddr.c_str());
    }
    return;
}

void addToCloudTask(char* payload);
void ProcessSendPacket(JsonObject& inJson) {
    char payload[300];
    inJson.printTo(payload, 300);
    // gUpdateCloud = true;
    addToCloudTask(payload);
}

#endif
