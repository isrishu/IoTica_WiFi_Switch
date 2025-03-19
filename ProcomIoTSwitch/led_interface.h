#ifndef __LED_INTERFACE_H__
#define __LED_INTERFACE_H__

#define FASTLED_RMT_MAX_CHANNELS 2  //Restict to use only 2 RMT channels to drive LED's.
#define FASTLED_RMT_BUILTIN_DRIVER 1 //this is manadatory as RMT is also used by IR remote

#define FASTLED_INTERNAL
#include <FastLED.h>

Ticker StaLedTicker;

float StaLedFastPeriod = 0.15;  //0.15 ->150 ms
float StaLedSlowPeriod = 0.5;  //0.5 ->500 ms

Ticker ledOnTicker;
Ticker ledOffTicker;
Ticker ledSleepTicker;

float ledOnTickerPeriod = 0.25;
float ledSleepTickerPeriod = 1;

Ticker otaLedTicker;
float otaLedPeriod = 0.5;  //0.5 ->500 ms

boolean sleepAllLeds= false;
boolean sleepStatus = false;
boolean ledOnTickerRunning = false;
boolean ledOffTickerRunning = false;
boolean pwrkey_handleInprogress = false;
bool isStaMqttConnUpdated = false;
bool isStaMqttDisConnUpdated = false;

volatile uint8_t globalStatusLED = 0;
uint8_t lastStatusLED = 0;

//otaLedTicker.attach(otaLedPeriod, StaLedTimerHandle);
//otaLedTicker.detach();

#define LED_TRUNOFF 0x000000
#define WHITE_COLOR 0xFFFFFF
#define GRAY_COLOR 0x404040
#define RED_COLOR 0x550000
#define GREEN_COLOR 0x005500
#define BLUE_COLOR 0x000055
#define YELLOW_COLOR_LOW 0xFFFFE0
#define YELLOW_COLOR 0x8B8000
#define ORENGE_COLOR 0x8B4000
#define VIOLET_COLOR 0x8F00FF
#define BLUE_COLOR_LOW 0x000005  // for sleep color (BLUE)
#define RED_COLOR_LOW 0x050000   // for sleep color (RED)
#define PINK_COLOR 0xDE0C78

#define FANSPEED_1_COLOR 0x004200
#define FANSPEED_2_COLOR 0x1A3800
#define FANSPEED_3_COLOR 0x242400
#define FANSPEED_4_COLOR 0x381A00
#define FANSPEED_5_COLOR 0x420000

#define LED_ON_COLOR  BLUE_COLOR  //cP.led_on_color
#define LED_OFF_COLOR  GRAY_COLOR //RED_COLOR//cP.led_off_color //GRAY_COLOR

#define MASTER_OFF_COLOR RED_COLOR

#define RESET_MODE_COLOR RED_COLOR
#define CONFIG_MODE_COLOR ORENGE_COLOR
#define WIFI_DISCONNECTED_COLOR LED_OFF_COLOR
#define WIFI_CONNECTED_COLOR GREEN_COLOR
#define MQTT_DISCONNECTED_COLOR WIFI_CONNECTED_COLOR // MQTT disconect or Wifi Connected
#define MQTT_CONNETCED_COLOR BLUE_COLOR
#define DEVICE_REGISTER_COLOR PINK_COLOR
#define OTA_UPDATE_COLOR YELLOW_COLOR

#ifdef TOUCH_8_MODULE
enum ledName
{
  led_R1,  //0
  led_R3,  //1
  led_PWR, //2
  led_R6,  //3
  led_FU,  //4
  led_F5,  //5
  led_F4,  //6
  led_F3,  //7
  led_F2,  //8
  led_F1,  //9
  led_FD,  //10
  //led_F,   //11
  led_R7,  //12
  led_R5,  //13
  led_R4,  //14
  led_R2,  //15
  led_STA, //16
  led_MAX, 
};
#endif

#ifdef TOUCH_4_MODULE
enum ledName
{
  led_R1,  //0
  led_R3,  //1
  led_FU,  //2
  led_F5,  //3
  led_F4,  //4
  led_F3,  //5
  led_F2,  //6
  led_F1,  //7
  led_FD,  //8
  //led_F,   //9
  led_R4,  //10
  led_R2,  //11
  led_STA, //12
  //--------------
  //led_PWR, //13
  //led_R5,  //14
  //led_R6,  //15
  //led_R7,  //16
  led_MAX, 
};
#endif

#ifdef TOUCH_2_MODULE
enum ledName
{
  led_R1,  //0
  led_FU,  //1
  led_R3,  //2
  led_R4,  //3
  led_FD,  //4
  led_R2,  //5
  led_STA, //6
  led_MAX, //7
};
#endif

#ifdef TOUCH_2_MODULE_BELL
enum ledName
{
  led_FU,  //0 //dummy
  led_R1,  //1
  led_R3,  //2 //dummy
  led_R4,  //3 //dummy
  led_R2,  //4
  led_FD,  //5 //dummy
  led_STA, //6
  led_MAX, //7
};
#endif

// Define the array of leds
CRGB leds[NUM_LEDS];

void updatePwrLedStatus();
void ledSleepEnable();

//#ifndef TOUCH_2_MODULE && #ifndef TOUCH_2_MODULE_BELL
#if !defined(TOUCH_2_MODULE) && !defined(TOUCH_2_MODULE_BELL)

void updateFanLED()
{
  
  if(fS.fan_1)
  {
    //leds[led_F]=  LED_TRUNOFF;
    leds[led_FU]= LED_ON_COLOR;
    leds[led_FD]= LED_ON_COLOR;
  }
  else
  {
    //leds[led_F]= LED_TRUNOFF;
    leds[led_FU]= LED_OFF_COLOR;
    leds[led_FD]= LED_OFF_COLOR;
  }
  
  if(fS.fan_1==5)
  {
    leds[led_F5]=FANSPEED_5_COLOR;
    leds[led_F4]=FANSPEED_4_COLOR;
    leds[led_F3]=FANSPEED_3_COLOR;
    leds[led_F2]=FANSPEED_2_COLOR;
    leds[led_F1]=FANSPEED_1_COLOR;
  }
  else if(fS.fan_1==4)
  {
    leds[led_F5]=LED_OFF_COLOR;
    leds[led_F4]=FANSPEED_4_COLOR;
    leds[led_F3]=FANSPEED_3_COLOR;
    leds[led_F2]=FANSPEED_2_COLOR;
    leds[led_F1]=FANSPEED_1_COLOR;
  }
  else if(fS.fan_1==3)
  {
    leds[led_F5]=LED_OFF_COLOR;
    leds[led_F4]=LED_OFF_COLOR;
    leds[led_F3]=FANSPEED_3_COLOR;
    leds[led_F2]=FANSPEED_2_COLOR;
    leds[led_F1]=FANSPEED_1_COLOR;
  }
  else if(fS.fan_1==2)
  {
    leds[led_F5]=LED_OFF_COLOR;
    leds[led_F4]=LED_OFF_COLOR;
    leds[led_F3]=LED_OFF_COLOR;
    leds[led_F2]=FANSPEED_2_COLOR;
    leds[led_F1]=FANSPEED_1_COLOR;
  }
  else if(fS.fan_1==1)
  {
    leds[led_F5]=LED_OFF_COLOR;
    leds[led_F4]=LED_OFF_COLOR;
    leds[led_F3]=LED_OFF_COLOR;
    leds[led_F2]=LED_OFF_COLOR;
    leds[led_F1]=FANSPEED_1_COLOR;
  }
  else
  {
    leds[led_F5]=LED_OFF_COLOR;
    leds[led_F4]=LED_OFF_COLOR;
    leds[led_F3]=LED_OFF_COLOR;
    leds[led_F2]=LED_OFF_COLOR;
    leds[led_F1]=LED_OFF_COLOR;
  }
}
#endif

void updateLedsOn()
{
    leds[led_R1]=(rS.relay1 ? LED_ON_COLOR : LED_OFF_COLOR);
    leds[led_R2]=(rS.relay2 ? LED_ON_COLOR : LED_OFF_COLOR);
    leds[led_R3]=(rS.relay3 ? LED_ON_COLOR : LED_OFF_COLOR);
    leds[led_R4]=(rS.relay4 ? LED_ON_COLOR : LED_OFF_COLOR);
    #ifdef TOUCH_2_MODULE_BELL
    leds[led_R3]= LED_TRUNOFF;
    leds[led_R4]= LED_TRUNOFF;
    #endif
    
    #ifdef TOUCH_8_MODULE
    leds[led_R5]=(rS.relay5 ? LED_ON_COLOR : LED_OFF_COLOR);
    leds[led_R6]=(rS.relay6 ? LED_ON_COLOR : LED_OFF_COLOR);
    leds[led_R7]=(rS.relay7 ? LED_ON_COLOR : LED_OFF_COLOR);
    leds[led_PWR]=(pS ? LED_ON_COLOR : MASTER_OFF_COLOR);
    #endif

    //#ifndef TOUCH_2_MODULE && ifndef TOUCH_2_MODULE_BELL
    #if !defined(TOUCH_2_MODULE) && !defined(TOUCH_2_MODULE_BELL)
    updateFanLED();
    #endif
    
    // Logger.Info("globalStatusLed: %d", globalStatusLED);
    setStaled(globalStatusLED);
    
    FastLED.show();
    delay(1);  //each LED takes 30uS, so 17*30uS = 510uS delay.
    ledSleepEnable();
}

void updateSleepLeds()
{
    leds[led_R1]=LED_TRUNOFF;
    leds[led_R2]=LED_TRUNOFF;
    leds[led_R3]=LED_TRUNOFF;
    leds[led_R4]=LED_TRUNOFF;
    
    #ifdef TOUCH_8_MODULE
    leds[led_R5]=LED_TRUNOFF;
    leds[led_R6]=LED_TRUNOFF;
    leds[led_R7]=LED_TRUNOFF;
    //leds[led_PWR]=(pS ? LED_ON_COLOR : MASTER_OFF_COLOR);
    leds[led_PWR]=(pS ? BLUE_COLOR_LOW : RED_COLOR_LOW);
    #endif

    //#ifndef TOUCH_2_MODULE && ifndef TOUCH_2_MODULE_BELL
    #if !defined(TOUCH_2_MODULE) && !defined(TOUCH_2_MODULE_BELL)
    leds[led_FU]=LED_TRUNOFF;
    leds[led_FD]=LED_TRUNOFF;
    leds[led_F1]=LED_TRUNOFF;
    leds[led_F2]=LED_TRUNOFF;
    leds[led_F3]=LED_TRUNOFF;
    leds[led_F4]=LED_TRUNOFF;
    leds[led_F5]=LED_TRUNOFF;
    #endif
    
    leds[led_STA]=LED_TRUNOFF;
    //Logger.Info("Updating LEDs ...............");
    
    FastLED.show();
    delay(1);  //each LED takes 30uS, so 17*30uS = 510uS delay.
}
void flushLed()
{
  //Logger.Info("LED flushing.............");
  FastLED.show();
  delay(1);
  ledSleepEnable();
}

void otaLedTimerHandle()
{
    if (isConfigMode) {
        return;
    }
    staLedUpdated = true;
    if(leds[led_STA] == CRGB(OTA_UPDATE_COLOR))
    { 
      leds[led_STA]= LED_OFF_COLOR;
    }
    else
    {
      leds[led_STA]= (OTA_UPDATE_COLOR);
    }
    FastLED.show();
}

void toggleStatusLed()
{
    if (isConfigMode) {
        return;
    }
    staLedUpdated = true;
    if(leds[led_STA] == CRGB(LED_OFF_COLOR))
    {
      leds[led_STA]=WIFI_CONNECTED_COLOR;
    }
    else
    {
      leds[led_STA]= (LED_OFF_COLOR);
    }
    FastLED.show();
}

void toggleConfigLed()
{
    staLedUpdated = true;
    if(leds[led_STA] == CRGB(CONFIG_MODE_COLOR))
    {
      leds[led_STA] = LED_OFF_COLOR;
    }
    else
    {
      leds[led_STA]= (CONFIG_MODE_COLOR);
    }

    FastLED.show();
}

void toggleWiFiLed()
{
    staLedUpdated = true;
    if(leds[led_STA] == CRGB(MQTT_DISCONNECTED_COLOR))
    {
      leds[led_STA] = (LED_OFF_COLOR);
    }
    else
    {
      leds[led_STA]= (MQTT_DISCONNECTED_COLOR);
    }

    FastLED.show();
}

void toggleRegisterLed()
{
    staLedUpdated = true;
    if(leds[led_STA] == CRGB(DEVICE_REGISTER_COLOR))
    {
      leds[led_STA] = LED_OFF_COLOR;
    }
    else
    {
      leds[led_STA]= (DEVICE_REGISTER_COLOR);
    }
    FastLED.show();
}

void toggleMQTTLed()
{
    if (isConfigMode)
    {
        return;
    }
    staLedUpdated = true;
    if(leds[led_STA] == CRGB(MQTT_CONNETCED_COLOR))
    {
      leds[led_STA] = LED_OFF_COLOR;
    }
    else
    {
      leds[led_STA]= (MQTT_CONNETCED_COLOR);
    }
    FastLED.show();
}

void toggleResetLed()
{
    staLedUpdated = true;
    if(leds[led_STA] == CRGB(RESET_MODE_COLOR))
    {
      leds[led_STA] = LED_OFF_COLOR;
    }
    else
    {
      leds[led_STA]= (RESET_MODE_COLOR);
    }
    FastLED.show();
}

boolean setStaled(int led_status)
{
    boolean LedsetStatus = 0;

    switch (led_status)
    {
    case STA_RESET_MODE:
      if(leds[led_STA] != CRGB(RESET_MODE_COLOR))
      {
        Logger.Info("Resetting Device Mode...!");
        leds[led_STA] = (RESET_MODE_COLOR);
        LedsetStatus = 1;
      }
      break;

    case STA_CONFIG_MODE:
      if(leds[led_STA] != CRGB(CONFIG_MODE_COLOR))
      {
        Logger.Info("Configuration Setup Mode...!");
        leds[led_STA] = (CONFIG_MODE_COLOR);
        LedsetStatus = 1;
      }
      break;

    case STA_CONFIG_WiFi_CONNECT_MODE:
      if(leds[led_STA] != CRGB(WIFI_CONNECTED_COLOR))
      {
        Logger.Info("Configuration WiFi Connecting Mode...!");
        leds[led_STA] = (WIFI_CONNECTED_COLOR);
        LedsetStatus = 1;
      }
      break;

    case STA_CONFIG_REGISTER_MODE:
      if(leds[led_STA] != CRGB(DEVICE_REGISTER_COLOR))
      {
        Logger.Info("Configuration Register Mode...!");
        leds[led_STA] = (DEVICE_REGISTER_COLOR);
        LedsetStatus = 1;
      }
      break;

    case STA_WIFI_DISCONNECT:
      if(leds[led_STA] != CRGB(LED_OFF_COLOR))
      {
        Logger.Info("Setting LED to WIFi Disconnected..");
        leds[led_STA] = (LED_OFF_COLOR);
        LedsetStatus = 1;
      }
      break;

    case STA_WIFI_CONNECTED:
    case STA_MQTT_DISCONNECT:
      if(hasDeviceID)
      {
        if(leds[led_STA] != CRGB(MQTT_DISCONNECTED_COLOR))
        {
          leds[led_STA] = (MQTT_DISCONNECTED_COLOR);
          LedsetStatus = 1;
        }
      }
      else
      {
        if(leds[led_STA] != CRGB(DEVICE_REGISTER_COLOR))
        {
          Logger.Info("Device not registered...!");
          leds[led_STA] = (DEVICE_REGISTER_COLOR);
          LedsetStatus = 1;
        }
      }
      break;

    case STA_MQTT_CONNECTED:
      if(leds[led_STA] != CRGB(MQTT_CONNETCED_COLOR))
      {
        leds[led_STA] = (MQTT_CONNETCED_COLOR);
        LedsetStatus = 1;
      }
      break;
    
    default:
      if(leds[led_STA] != CRGB(LED_OFF_COLOR))
      {
        leds[led_STA] = (LED_OFF_COLOR);
        LedsetStatus = 1;
      }
      break;
    }

    lastStatusLED = leds[led_STA];

    if (sleepStatus && !isConfigMode) //if ALL LEDS are sleeping then Dont run status LED Enable sequence.
    { 
        LedsetStatus = 0;
    }

    if (LedsetStatus) 
    {
      ledRunStatus.devRegister = false;
    }
    return LedsetStatus;
}

void restoreLedOn()
{  
  leds[led_R1]= (LED_OFF_COLOR);
  leds[led_R2]= (LED_OFF_COLOR);
  leds[led_R3]= (LED_OFF_COLOR);
  leds[led_R4]= (LED_OFF_COLOR);
  #ifdef TOUCH_2_MODULE_BELL    
  leds[led_R3]= (LED_TRUNOFF);
  leds[led_R4]= (LED_TRUNOFF);
  #endif
  
  #ifdef TOUCH_8_MODULE
  leds[led_R5]= (LED_OFF_COLOR);
  leds[led_R6]= (LED_OFF_COLOR);
  leds[led_R7]= (LED_OFF_COLOR);
  //leds[led_PWR]= (MASTER_OFF_COLOR);
  #endif

  //#ifndef TOUCH_2_MODULE && ifndef TOUCH_2_MODULE_BELL
  #if !defined(TOUCH_2_MODULE) && !defined(TOUCH_2_MODULE_BELL)
  //leds[led_F]= (LED_TRUNOFF);
  leds[led_FU]= (LED_OFF_COLOR);
  leds[led_FD]= (LED_OFF_COLOR);

  leds[led_F1]= (LED_OFF_COLOR);
  leds[led_F2]= (LED_OFF_COLOR);
  leds[led_F3]= (LED_OFF_COLOR);
  leds[led_F4]= (LED_OFF_COLOR);
  leds[led_F5]= (LED_OFF_COLOR);
  #endif

  Logger.Info("LED TurOff.............");
  FastLED.show();
  delay(1);
  ledSleepEnable();
    
}


void updateLedsOff()
{
    
    leds[led_R1]= (LED_OFF_COLOR);
    leds[led_R2]= (LED_OFF_COLOR);
    leds[led_R3]= (LED_OFF_COLOR);
    leds[led_R4]= (LED_OFF_COLOR);
    #ifdef TOUCH_2_MODULE_BELL    
    leds[led_R3]= (LED_TRUNOFF);
    leds[led_R4]= (LED_TRUNOFF);
    #endif
    
    #ifdef TOUCH_8_MODULE
    leds[led_R5]= (LED_OFF_COLOR);
    leds[led_R6]= (LED_OFF_COLOR);
    leds[led_R7]= (LED_OFF_COLOR);
    leds[led_PWR]= (MASTER_OFF_COLOR);
    #endif

    //#ifndef TOUCH_2_MODULE && ifndef TOUCH_2_MODULE_BELL
    #if !defined(TOUCH_2_MODULE) && !defined(TOUCH_2_MODULE_BELL)
    //leds[led_F]= (LED_TRUNOFF);
    leds[led_FU]= (LED_OFF_COLOR);
    leds[led_FD]= (LED_OFF_COLOR);

    leds[led_F1]= (LED_OFF_COLOR);
    leds[led_F2]= (LED_OFF_COLOR);
    leds[led_F3]= (LED_OFF_COLOR);
    leds[led_F4]= (LED_OFF_COLOR);
    leds[led_F5]= (LED_OFF_COLOR);
    #endif

    Logger.Info("LED TurOff.............");
    FastLED.show();
    delay(1);
    ledSleepEnable();
    
}

int LED_SLEEP_TIME = 10; //in seconds

uint8_t sleepTimeCounter = 0;
void ledSleepTickerHandle()
{
   sleepTimeCounter++;
   if(sleepTimeCounter > LED_SLEEP_TIME && !powerkeyIsinTouch && !fanDownkeyIsinTouch && !isConfigMode)
   {
       sleepAllLeds= true;
       sleepStatus = true;
       sleepTimeCounter=0;
       ledSleepTicker.detach();
   }
}

void ledSleepEnable()
{
  ledSleepTicker.detach();
  ledSleepTicker.attach(ledSleepTickerPeriod, ledSleepTickerHandle);
  sleepTimeCounter = 0; 
}

void staLedDetach() 
{
  StaLedTicker.detach();
  ledSleepEnable();
}

void staLedAttach (led_ticker_speed speed, sta_led staLed)
{
  staLedDetach();
  float period = (speed == TICKER_SLOW) ? StaLedSlowPeriod : StaLedFastPeriod;
  staLedUpdated = setStaled(staLed);
  globalStatusLED = staLed;

  switch (staLed)
  {
    case STA_WIFI_CONNECTED:
    case STA_CONFIG_WiFi_CONNECT_MODE:
      StaLedTicker.attach(period, toggleWiFiLed);
      break;
    case STA_CONFIG_MODE:
      StaLedTicker.attach(period, toggleConfigLed);
      break;
    case STA_DEVICE_REGISTER:
    case STA_CONFIG_REGISTER_MODE:
      StaLedTicker.attach(period, toggleRegisterLed);
      break;
    case STA_MQTT_CONNECTED:
      StaLedTicker.attach(period, toggleMQTTLed);
      break;
    case STA_RESET_MODE:
      StaLedTicker.attach(period, toggleResetLed);
      break;
    default:
      Logger.Info("StaLed tickering function not defined....");

  }

  if(!isConfigMode)
  {
    ledSleepEnable();
  }
}

static int relayNum=1;
void ledOnTickerHandle()
{
  
  // Logger.Info(relayNum);
  if(relayNum>8)
  {
    ledOnTicker.detach();
    relayNum=1;
    ledOnTickerRunning =false;
    pwrkey_handleInprogress =false;
    return;
  }
  switch(relayNum)
  {
    case 1:
          relayNum++;
          if(rS.relay1)
          {
            //changeRelayStatus(1, RLY_ON);
            leds[led_R1]= (LED_ON_COLOR);
            break;
          }
    case 2:
          relayNum++;
          if(rS.relay2)
          {
            //changeRelayStatus(2, RLY_ON);
            leds[led_R2]= (LED_ON_COLOR);
            break;
          }
    case 3:
          relayNum++;
          if(rS.relay3)
          {
            #ifndef TOUCH_2_MODULE_BELL
            leds[led_R3]= (LED_ON_COLOR);
            #endif
            break;
          }
    case 4:
          relayNum++;
          if(rS.relay4)
          {
            #ifndef TOUCH_2_MODULE_BELL
            leds[led_R4]= (LED_ON_COLOR);
            #endif
            break;
          }
    
    case 5:
          relayNum++;
          #ifdef TOUCH_8_MODULE
          if(rS.relay5)
          {
            leds[led_R5]= (LED_ON_COLOR);
            break;
          }
          #endif
     case 6:
          relayNum++;
          #ifdef TOUCH_8_MODULE
          if(rS.relay6)
          {
            leds[led_R6]= (LED_ON_COLOR);
            break;
          }
          #endif
     case 7:
          relayNum++;
          #ifdef TOUCH_8_MODULE
          if(rS.relay7)
          {
            leds[led_R7]= (LED_ON_COLOR);
            break;
          }
          #endif
     case 8:
          relayNum++;
          if(fS.fan_1)
          {
            //#ifndef TOUCH_2_MODULE && ifndef TOUCH_2_MODULE_BELL
            #if !defined(TOUCH_2_MODULE) && !defined(TOUCH_2_MODULE_BELL)
            updateFanLED();
            #endif
            
            break;
          }
     default:
          break;
  }
  //Logger.Info("LED On tick handle.............");
  FastLED.show();
  ledSleepEnable();
}

static int relayoffNum=1;
void ledOffTickerHandle()
{
  // Logger.Info("OFF Timer..");
  // Logger.Info(relayoffNum);
  if(relayoffNum>8)
  {
    ledOffTicker.detach();
    relayoffNum=1;
    ledOffTickerRunning =false;
    pwrkey_handleInprogress =false;
    return;
  }
  switch(relayoffNum)
  {
    case 1:
          relayoffNum++;
          if(rS.relay1)
          {
            //changeRelayStatus(1, RLY_OFF);
            leds[led_R1]= (LED_OFF_COLOR);
            break;
          }
    case 2:
          relayoffNum++;
          if(rS.relay2)
          {
            leds[led_R2]= (LED_OFF_COLOR);
            break;
          }
    case 3:
          relayoffNum++;
          if(rS.relay3)
          {
            #ifndef TOUCH_2_MODULE_BELL
            leds[led_R3]= (LED_OFF_COLOR);
            #endif
            break;
          }
    case 4:
          relayoffNum++;
          if(rS.relay4)
          {
            #ifndef TOUCH_2_MODULE_BELL
            leds[led_R4]= (LED_OFF_COLOR);
            #endif
            break;
          }
    #ifdef TOUCH_8_MODULE
    case 5:
          relayoffNum++;
          if(rS.relay5)
          {
            leds[led_R5]= (LED_OFF_COLOR);
            break;
          }
     case 6:
          relayoffNum++;
          if(rS.relay6)
          {
            leds[led_R6]= (LED_OFF_COLOR);
            break;
          }
     case 7:
          relayoffNum++;
          if(rS.relay7)
          {
            leds[led_R7]= (LED_OFF_COLOR);
            break;
          }
     #endif

     //#ifndef TOUCH_2_MODULE && ifndef TOUCH_2_MODULE_BELL
     #if !defined(TOUCH_2_MODULE) && !defined(TOUCH_2_MODULE_BELL)
     case 8:
          relayoffNum++;
          if(fS.fan_1)
          {
            //leds[led_F]= (LED_TRUNOFF);
            leds[led_FU]= (LED_OFF_COLOR);
            leds[led_FD]= (LED_OFF_COLOR);
            leds[led_F1]= (LED_OFF_COLOR);
            leds[led_F2]= (LED_OFF_COLOR);
            leds[led_F3]= (LED_OFF_COLOR);
            leds[led_F4]= (LED_OFF_COLOR);
            leds[led_F5]= (LED_OFF_COLOR);
            break;
          }
     #endif
     default:
          relayoffNum++;
          break;
  }
//  Logger.Info("LED OFF tick handle.............");
  FastLED.show();
  ledSleepEnable();
}



void updatePonLedStatus()
{
  updateLedsOff();
  //ledOnTicker.attach(ledOnTickerPeriod, ledOnTickerHandle);
  delay(100);
  updatePwrLedStatus();
}

void updatePwrLedStatus()
{
   
   if(pS)
   {
      #ifdef TOUCH_8_MODULE
      leds[led_PWR]= (LED_ON_COLOR);
      restoreLedOn();
      #endif
      ledOnTickerRunning =true;
      ledOnTicker.attach(ledOnTickerPeriod, ledOnTickerHandle);
   }
   else
   {
      #ifdef TOUCH_8_MODULE
      leds[led_PWR]= (MASTER_OFF_COLOR);
      updateLedsOn();
      #endif
      ledOffTickerRunning = true;
      ledOffTicker.attach(ledOnTickerPeriod, ledOffTickerHandle);
   }
   //#endif
}


void updateAllLedStatus()
{
    if(pS)
    {
        sleepStatus=false;
        LED_SLEEP_TIME = 10;
        updateLedsOn();
    }
    else
    {
        updateLedsOff();
    }
}

void setup_led() { 
      FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
}

void loop_led() { 
  //Do LED update, if we see any failure in light. Optional.
  if(sleepAllLeds && !powerkeyIsinTouch && !fanDownkeyIsinTouch)
  {
    StaLedTicker.detach();
    updateSleepLeds();
    sleepAllLeds=false;
  }

  return;
}  

#endif
