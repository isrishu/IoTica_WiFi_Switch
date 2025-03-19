#ifndef __COMMON_H__
#define __COMMON_H__

// #include "AsyncUDP.h"
#include "module_config.h"
#include "Macros.h"
#include "SerialLogger.h"

uint8_t taskCount = 0;
bool isUpdateAborted = false;
boolean hasDeviceID = false;
boolean hasConfigured = false;
#define MAX_RETRY 5
uint8_t retry_count = 0;

#define CONNECTIVITY_CHECK_INTERVAL        60 // 10s

// AsyncUDP udp;
#define MESSAGE_MAX_LEN 256
#define ACK_MAX_LEN 32
// #define UDP_MESSAGE_MAX_LEN 200
// bool isUDPSetup = false;

// const char *outPayload = "{\"commandToExecute\":\"%s\",\"deviceID\":\"%s\",\"packet\":\"%s\"}";
const char *outPayload = "{\"deviceID\":\"%s\",\"packet\":\"%s\",\"ack\":\"%s\"}";
char ack_type[ACK_MAX_LEN];

struct TaskContext;
void abortTask(void *ctx);
void destroyTask(TaskContext *taskContext);
void finishTask(void *ctx);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define JSON_VALIDATE_AND_COPY_PARAM(key, value) \
    do { \
        const char *data = (value); \
        if((data)) {\
            strcpy((key), (value)); \
        } \
    } while (0);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define JSON_VALIDATE_AND_COPY_PARAM_INT(key, value) \
    do { \
        (key) = ((value) > 0) ? (value) : (key) ;    \
    } while (0);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define JSON_VALIDATE_AND_RESET_PARAM(cond, resetkey) \
    do { \
        const char *data = (cond); \
        if((data)) {\
            memset((resetkey), 0, sizeof(resetkey)); \
        } \
    } while (0);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SET_BIT(num, pos) ((num) |= (1U << (pos)))
#define CLEAR_BIT(num, pos) ((num) &= (~(1U << (pos))))
#define TOGGLE_BIT(num, pos) ((num) ^= (1U << (pos)))
#define ASSIGN_BIT(num, pos, val) ((num) = (val)?SET_BIT((num), (pos)):CLEAR_BIT((num), (pos)))
#define CMP_BIT(num, pos, val) ((((num) >> (pos)) & 1U) == (val))
#define GET_BIT(num, pos) (((num) >> (pos)) & 1U)

#define ASSIGN_NIBBLE(num, pos, val) ((num) = ((num) & (0xF0 >> (4 * (pos)))) | ((val) << (4 * (pos))))
#define CMP_NIBBLE(num, pos, val) ((((num) >> (4 * (pos))) & 0x0F) == (val))
#define GET_NIBBLE(num, pos) ((pos) ? ((num) >> 4) : ((num) & 0x0F))

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void updateByteFromArray(uint8_t *inPacket, uint8_t len, uint8_t *value) {
    uint8_t outValue = *value;
    for (uint8_t i = 0; i < len; i++) {
        // if (inPacket[i]) {
        //     SET_BIT(outValue,i);
        // } else {
        //     CLEAR_BIT(outValue,i);
        // }
        ASSIGN_BIT(outValue, i, inPacket[i]);
    }
    *value = outValue;
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void getArrayFromByte(uint8_t inValue, uint8_t *outPacket, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        outPacket[i] = (1 & (inValue >> i));
    }

    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void updateNibbleFromArray(uint8_t *inPacket, uint8_t len, uint8_t *value) {
    uint8_t outValue = *value;
    for (uint8_t i = 0; i < len; i++) {
        outValue |= (inPacket[i] << (i * 4));
    }
    *value = outValue;
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void getArrayFromNibble(uint8_t inValue, uint8_t *outPacket, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        outPacket[i] = 0x0F & (inValue >> (i * 4));
    }

    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void printArray(const char *fun_name, uint8_t *inPacket, uint8_t length) {
    Logger.Info("%s::Len(%d) Data:", fun_name, length);
    for (int i = 0; i < length; i++) {
        Logger.print("%d ", inPacket[i]);
    }
    Logger.println();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SIZEOF_INT sizeof(int)
#define delim " "
 uint8_t * stringToNumArray (char str[], uint8_t *plength) {
    // uint8_t init_size = strlen(str);
    uint8_t len = 0;
    static uint8_t inPacket[IN_PACKET_ARRAY_MAX];

    char *ptr = strtok(str, delim);
    while(ptr != NULL)
    {
        inPacket[len] = atoi(ptr);
        len++;
        ptr = strtok(NULL, delim);
    }

    *plength = len;
    // printArray(__FUNCTION__, inPacket, *plength);

	return inPacket;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////

void numArrayToString(uint8_t *data, uint32_t len, char *str, uint32_t *poutLen) {
    uint32_t str_len = 0;

    for (uint8_t i = 0; i < len; i++) {
        char in[SIZEOF_INT];
        snprintf(in, SIZEOF_INT, "%d", data[i]);
        uint32_t in_len = strlen(in);
        // printf("%d '%s' %d\n", data[i], in, in_len);
        if (i != 0) {
            str[str_len++] = ' ';
        }

        memcpy(&str[str_len], in, in_len);
        str_len += in_len;
    }

    // printf("Output: %s Len: %d\n", str, str_len);
    *poutLen = str_len;
	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void constructOutMessagePayload(char* deviceId, char* packet, char messagePayload[]) {
    snprintf(messagePayload, MESSAGE_MAX_LEN, outPayload, deviceId, packet, ack_type); /*store switch status in messagePayload*/
    Logger.Info("Request  Body --> %s" , messagePayload);
    memset(ack_type, ACK_MAX_LEN, 0);
    memcpy(ack_type, ACK_DEVICE, ACK_MAX_LEN);
    return ;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
