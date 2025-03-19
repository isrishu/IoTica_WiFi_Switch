#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#define SOFTWARE_VER "2.3.13"
#define LAST_UPDATED "13/09/2024-0"
#define ADMIN_USERNAME "admin"
#define ADMIN_PASSWORD "smart"

#define TOUCH_8_MODULE
//#define TOUCH_4_MODULE
//#define TOUCH_2_MODULE
//#define TOUCH_2_MODULE_BELL

#define APP_BAUDRATE  115200

#define PWR_BRD_RESET_PIN 23 

//Long press power button to goto configuration mode
#define CONFIG_TIME (6*1000) //6 seconds

//Touch Config parametersprocom/47c6eb6fccee/events/control
#define CAP_INTR_PIN 34
#define ESP_CAP_RESET 27 //Pull low to enable Touch IC CAP1114.
#define CAP_I2CADDR 0x28


//IR Config parameters
uint16_t RECV_PIN = 35;
#define IR_RMT_CHANNEL RMT_CHANNEL_6

//LED Config parameters
#define LED_DATA_PIN 25

#ifdef TOUCH_8_MODULE
#define NUM_LEDS 17
#endif

#ifdef TOUCH_4_MODULE
#define NUM_LEDS 13
#endif

#ifdef TOUCH_2_MODULE
#define NUM_LEDS 7
#endif

#ifdef TOUCH_2_MODULE_BELL
#define NUM_LEDS 7
#endif

//Wifi Config parameters
#define WIFI_SSID  cP.wifi_ssid
#define WIFI_PASSWORD cP.wifi_pwd

//Power board Config parameters
#define I2C_SDA 21 //23
#define I2C_SCL 22 //22

//#define MQTT_LOG_ENABLE

#ifdef MQTT_LOG_ENABLE
//String to store Serial data
String logData = "";
#endif
String Config_SSID ="";
String Config_pass="";


#endif
