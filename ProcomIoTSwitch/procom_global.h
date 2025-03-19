#ifndef __PROCOM_GLOBAL_H__
#define __PROCOM_GLOBAL_H__

extern "C" {
  #include "freertos/FreeRTOS.h"
  //#include "freertos/timers.h"
  #include "driver/touch_pad.h"
  #include "soc/rtc_cntl_reg.h"
  #include "soc/sens_reg.h"
}

//Global Variables
enum loop_cmd
{
   DO_NOTHING = 0,
   DO_SMARTCONFIG,
   DO_RESET,
   DO_OTA_UPDATE
};

loop_cmd loopCommand = DO_NOTHING;
int wifiCommand = 0;

//below are the global structure which stores the global data
struct relayStatusStruct rS = {RLY_OFF, RLY_OFF, RLY_OFF, RLY_OFF, RLY_OFF, RLY_OFF, RLY_OFF, 0};
struct fanStatusStruct fS = {FAN_OFF,FAN_OFF};

//#ifdef TOUCH_4_MODULE
//uint8_t pS =1;
//#else
static uint8_t pS =0;
//#endif

volatile boolean gUpdateRelay = false;
volatile boolean gUpdateFan = false;
boolean gUpdateCloud = false;
volatile boolean isFromDevice = false;
volatile boolean isFromCloud = false;

static int fan1_prev_status=0;
static int fan2_prev_status=0;
static uint8_t mainpowerStatus=0;
volatile boolean mainpowerStatusUpdated=false;

enum fanModeCtrl
{
  FAN_CTRL_OFF,
  FAN_CTRL_ON,
  FAN_CTRL_TOGGLE,
  FAN_CTRL_UP,
  FAN_CTRL_DOWN
};

enum mainPowerCtrl
{
  MAIN_PWR_OFF,
  MAIN_PWR_ON,
  MAIN_PWR_TOGG
};

enum sta_led
{
  STA_NO_OPP_MODE = 0,
  STA_RESET_MODE,
  STA_CONFIG_MODE,
  STA_CONFIG_REGISTER_MODE,
  STA_CONFIG_WiFi_CONNECT_MODE,
  STA_WIFI_DISCONNECT,
  STA_WIFI_CONNECTED,
  STA_MQTT_DISCONNECT,
  STA_MQTT_CONNECTED,
  STA_DEVICE_REGISTER
};

enum led_ticker_speed
{
  TICKER_SLOW,
  TICKER_FAST
};

//Function declarations
void changeRelayStatus(uint8_t relayNo, uint8_t relStatus);
void changeFanStatus(uint8_t fanNo, uint8_t fanOnOFF);
void changeFanMode(uint8_t fanNo, uint8_t fanModectrl);
void changeMainPowerStatus(uint8_t Powerstatus);
void updateLedsOn();
void setFanSpeed(uint8_t, uint8_t);
boolean setStaled(int);
void triggerCallingBell();

#endif
