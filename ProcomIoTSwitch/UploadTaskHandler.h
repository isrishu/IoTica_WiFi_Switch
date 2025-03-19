#ifndef __UPLOAD_TASK_HANDLER_H__
#define __UPLOAD_TASK_HANDLER_H__

#include "freertos/FreeRTOS.h"
#include "AsyncHttpPost.h"
#include "ModuleHandler.h"
#include "parseHttpResponse.h"

#define registerDeviceApi "/api/notifyDeviceRegisteredWithIoTHub?code=3awagIrcEk02S5mS95PT2h5aMNbNC4rMlvkfZw1AW9qDAzFumFPwNg\%3D\%3D"
#define toggleSwitchApi "/api/toggleSwitches?code=lE4NFecGBO5s0SRgOzBTQsJI6cJBaX7ivk8Zq9jI-5hsAzFuldxWrQ\%3D\%3D"
#define procom_azure_url "procomfunctionapp.azurewebsites.net"

uint8_t taskIndex = 0;
TaskContext gTaskList[TASK_MAX] = {0};

long time_last_touch;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void destroyTask(void *args) {
    if (!args) return;
    TaskContext *taskContext = (TaskContext *)args;
    int task_no = taskContext->taskNo;

    Logger.Info("Task %d destroyed\n", task_no);
    // memset((void *)&gTaskList[task_no], 0, sizeof(TaskContext));
    taskCount--;
    if (taskCount <= 0) {
        taskIndex = 0;
        taskCount = 0;
    }

    if (taskContext->reqParams.client.connected())
    {
        taskContext->reqParams.client.close();
        delay(50);
    } 

    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void abortTask(void *args) {
    if (!args) return;
    TaskContext *taskContext = (TaskContext *)args;
    long timeNow = millis();
    Logger.Info("Task: %d aborted, Resp Timeout %d ms\n", taskContext->taskNo, (int)(timeNow - taskContext->timeBegin));
    taskContext->taskStatus = TASK_ABORTED;
    destroyTask(args);
    isUpdateAborted = true;
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void finishTask(void *args) {
    if (!args) return;
    TaskContext *taskContext = (TaskContext *)args;
    long timeNow = millis();

    Logger.Info("Task: %d finished, Resp Time %d ms\n", taskContext->taskNo, (int)(timeNow - (taskContext->timeBegin)));
    taskContext->taskStatus = TASK_SUCCESS;
    destroyTask(args);
    return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void createContext(uint8_t taskNo, char* payload) {
    // memset((void *)&gTaskList[taskNo], 0, sizeof(TaskContext));
    long timeNow = millis();
    time_last_touch = timeNow;
    TaskContext *taskContext = &gTaskList[taskNo];
    taskContext->taskNo = taskNo;
    taskContext->timeBegin = timeNow;

    taskContext->taskStatus = TASK_STARTED;

    if (isAsycTCPConfigured == false) {
        taskContext->reqParams.client = asyncClient;
        taskContext->reqParams.length = MESSAGE_MAX_LEN;
        taskContext->reqParams.callback = parseDataFromHttpPostResponse;
        taskContext->reqParams.api = toggleSwitchApi;
        taskContext->reqParams.host = procom_azure_url;
        config_http_post((void *) taskContext);

        isAsycTCPConfigured = true;
    }

    memset(taskContext->reqParams.data, 0, MESSAGE_MAX_LEN);
    memcpy(taskContext->reqParams.data, payload, taskContext->reqParams.length);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void addToCloudTask(char* payload) {
    uint8_t taskNo = (taskIndex++) % TASK_MAX;
    if (gTaskList[taskNo].taskStatus == TASK_RUNNING) {
        abortTask((void*)&gTaskList[taskNo]);
        // memset((void *)&gTaskList[taskNo], 0, sizeof(TaskContext));
    }
    createContext(taskNo, payload);
    Logger.Info("**** Task %d added ****", gTaskList[taskNo].taskNo);

    sendHttpsPostRequest((TaskContext*)&gTaskList[taskNo]);
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void publishStatusPayLoad(void) {
    uint8_t length = 0;
    uint32_t outPackLen = 0;
    if(gUpdateCloud) {
        uint8_t packet[OUT_PACKET_ARRAY_MAX] = {0};
        convertModuleToPacket(packet, &length);
        // printArray(__FUNCTION__, packet, length);
        if (length > 0) {
            char packet_str[PACKET_STRING_MAX] = {0};
            numArrayToString(packet, length, packet_str, &outPackLen);
            Logger.Info("Data(%d): %s", outPackLen, packet_str);

            char payLoad[MESSAGE_MAX_LEN] = {0};
            constructOutMessagePayload(cP.deviceID, packet_str, payLoad);
            // if (isUDPSetup) {
            //     udp.broadcast(payLoad);
            // }
            Logger.Info("Data: %s", payLoad);
            addToCloudTask(payLoad);
        }

        gUpdateCloud = false;
    } 
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif