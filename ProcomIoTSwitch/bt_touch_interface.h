#ifndef __BT_TOUCH_INTERFACE_H__
#define __BT_TOUCH_INTERFACE_H__

#include "bt_cap1114.h"

#ifdef TOUCH_8_MODULE
#define POWERKEY_DATA 4096 //0x800
#endif

#ifdef TOUCH_4_MODULE
#define POWERKEY_DATA 2048
#endif

#ifdef TOUCH_2_MODULE
#define POWERKEY_DATA 4096
#endif

#ifdef TOUCH_2_MODULE_BELL  //not used..
#define POWERKEY_DATA 2048
#endif

#ifdef TOUCH_8_MODULE
#define FANDOWNKEY_DATA 4
#endif

#ifdef TOUCH_4_MODULE
#define FANDOWNKEY_DATA 4
#endif

#ifdef TOUCH_2_MODULE
#define FANDOWNKEY_DATA 4
#endif

#ifdef TOUCH_2_MODULE_BELL
#define FANDOWNKEY_DATA 4
#endif

#define SMART_CONFIG_PRESS_TIME 5 //sec
#define SPIFFS_RESET_PRESS_TIME 10 //sec
#define SKIP_CONFIG_PRESS_TIME 15 //sec
#define FANDOWNKEY_PRESS_TIME 2 //sec

Ticker powerTicker;
Ticker fanDownTicker;
float powerTickerPeriod=1;
float fanDownTickerPeriod=0.3;
int powerkeyModeNo=0;
int fanDownkeyModeNo=0;
boolean updateFanToggle=false;

//Enums and structures
enum error_status
{
  BT_SUCCESS,
  BT_FAIL,
  BT_HW_ERROR
};

struct touchStatus
{
  uint16_t touch1:1;
  uint16_t touch2:1;
  uint16_t touch3:1;
  uint16_t touch4:1;
  uint16_t touch5:1;
  uint16_t touch6:1;
  uint16_t touchDn:1;
  uint16_t touchUp:1;
  uint16_t touch7:1;
  uint16_t touch8:1;
  uint16_t touch9:1;
  uint16_t touch10:1;
  uint16_t touch11:1;
  uint16_t touch12:1;
  uint16_t touch13:1;
  uint16_t touch14:1;
};


//global variables
struct touchStatus *tS;
volatile boolean capInterruptCame=false;
unsigned long powerkeyTouchtime=0;
unsigned long fanDownkeyTouchtime=0;
boolean fanDownkeyUpdate = false;

void setup_touch();
void loop_touch();

//Local Functions
void handleTouchdata(uint16_t touchData);
uint16_t readTouchData(uint8_t i2c_addr);
void clearTouchInterrupt(uint8_t i2c_addr);
void processTouchInterrupt(uint8_t i2c_addr);
void IRAM_ATTR handleTouchInterrupt();
void PowerkeyTimer();
void fanDownkeyTimer();

//Function implimentation

uint16_t readTouchData(uint8_t i2c_addr)
{
    return readcap1114TouchData(i2c_addr);
    //return readcap1188TouchData(i2c_addr);
}

void clearTouchInterrupt(uint8_t i2c_addr)
{
  clearCap1114Interrupt(i2c_addr);
  //clearCap1188Interrupt(i2c_addr);
}

int touchCouter = 0;

void processTouchInterrupt(uint8_t i2c_addr)
{
  uint16_t prevStatus = readTouchData(i2c_addr);
  Logger.Info("(%d) Prev Touch Data: %d\n", touchCouter++, prevStatus);
  
  clearTouchInterrupt(i2c_addr);
  if (prevStatus == 0) {
     Logger.Warning("No touch detected, Invalid interrupt");
     return;
  }
  delay(2);
  
  uint16_t currStatus = readTouchData(i2c_addr);
  Logger.Info("Current Touch Data: %d", currStatus);
  
  //for touch 8 and 14 we should not read again, this always gives interrupt only when touch detected (check ungrouped behaviour in datasheet)
 // if( prevStatus== 512)
 // {
 //    currStatus = prevStatus;
 // }
  
  if(prevStatus==currStatus)
  {
    //it is a touch interrupt
    Logger.Info("Touch interrupt");
    char buffer[42];

    //Check for powr key long press.
    if(prevStatus== POWERKEY_DATA) // power key touch value
    {
      updateAllLedStatus();
      powerTicker.attach(powerTickerPeriod, PowerkeyTimer);
      powerkeyIsinTouch = true;
    }
    else if(prevStatus== FANDOWNKEY_DATA)  //-> remove it in next revision
    {
      updateAllLedStatus();
      fanDownTicker.attach(fanDownTickerPeriod, fanDownkeyTimer);
      fanDownkeyIsinTouch = true;
    }
    else
	  {
      handleTouchdata(currStatus);
	  }
 
    //Below is only for debug purpose.
    int touchNum=0;
    for (uint8_t i=0; i<16; i++) {
      if (prevStatus & (1 << i)) {
        if(i>7)
          touchNum=i-1;
        else
          touchNum=i+1;
        Logger.Info("C%d", touchNum);
        snprintf(buffer, 6, "C%d\t", touchNum);
      }
    }
    #ifdef MQTT_LOG_ENABLE
    logData = logData +"\n" + buffer;
    #endif
  }
  else
  {
    //it is a release interrupt
    Logger.Info("release interrupt");
    if(prevStatus == POWERKEY_DATA) // power key touch value
    {
      powerkeyIsinTouch = false;
      powerTicker.detach();
      if (powerkeyUpdate == RESET_DEVICE) 
      {
        Logger.Info("Enabling reset device");
        loopCommand = DO_RESET;
        staLedDetach();
      } 
      else if(powerkeyUpdate == SMART_CONFIG) //if it is more than 5 seconds
      {
        Logger.Info("Enabling smart config");
        loopCommand = DO_SMARTCONFIG;
	    }
      else if(powerkeyUpdate == RESTART_DEVICE) //if it is more than 5 seconds
      {
        Logger.Info("Restarting Device....");
        delay(1000);
        ESP.restart();
	    } 
      else
      {
        handleTouchdata(prevStatus);
      }
      powerkeyModeNo = 0;
      powerkeyUpdate = MODE_UNKNOWN;
    }
    if(prevStatus == FANDOWNKEY_DATA) // Fan key touch value
    {
      fanDownkeyIsinTouch = false;
      fanDownTicker.detach();
      if(fanDownkeyUpdate) //if it is more than 5 seconds
      {
        fanDownkeyUpdate = false;
        fanDownkeyModeNo = 0;
        
        Logger.Info("Turning off Fan");
        updateFanToggle = true;
      }
      else
      {
        handleTouchdata(prevStatus);
      }
    }
    else if(prevStatus==512) //no need we can remove it
    {
      handleTouchdata(prevStatus);
    }
  }
}

void handleTouchdata(uint16_t touchData)
{
   tS=(struct touchStatus *)&touchData;
     if(tS->touch1)
     {
        #ifdef TOUCH_8_MODULE
        if(pS){  
          //if(fS.fan_1!=FAN_OFF)
           changeFanMode(1, FAN_CTRL_UP);
        }
        #endif
        #ifdef TOUCH_4_MODULE
         //if(fS.fan_1!=FAN_OFF)
         changeFanMode(1, FAN_CTRL_UP);
        #endif
        #ifdef TOUCH_2_MODULE
          changeRelayStatus(1, RLY_TOG);
        #endif
     }
     if(tS->touch2)  //Fan down
     {
          /*
          #ifdef TOUCH_8_MODULE
          if(pS)
          changeFanStatus(1,FAN_CTRL_TOGGLE);
          #endif
          #ifdef TOUCH_4_MODULE
          changeFanStatus(1,FAN_CTRL_TOGGLE);
          #endif
          */
          #ifdef TOUCH_2_MODULE
            //changeRelayStatus(2, RLY_TOG);
            Logger.Info("Curtain 2 ");
          #endif
          #ifdef TOUCH_2_MODULE_BELL
            triggerCallingBell();
            Logger.Info("BELL Toggle ");
          #endif
     }
     if(tS->touch3)
     {
        
         #ifdef TOUCH_8_MODULE
          //if(fS.fan_1!=FAN_OFF)
          if(pS)
            changeFanMode(1, FAN_CTRL_DOWN);
         #endif
         #ifdef TOUCH_4_MODULE
         //if(fS.fan_1!=FAN_OFF)
         changeFanMode(1, FAN_CTRL_DOWN);
         #endif
         #ifdef TOUCH_2_MODULE
          changeRelayStatus(3, RLY_TOG);
        #endif 
        
     }
     if(tS->touch4)
     {
          #ifdef TOUCH_8_MODULE
          if(pS)
          changeRelayStatus(7, RLY_TOG);
          #endif
          
          //#ifdef TOUCH_4_MODULE
          //changeFanStatus(1,FAN_CTRL_TOGGLE);
          //#endif
          
          //#ifdef TOUCH_2_MODULE
          //changeRelayStatus(4, RLY_TOG);
          //#endif
     }
     if(tS->touch5)
     {
          #ifdef TOUCH_8_MODULE
          if(pS)
            changeRelayStatus(5, RLY_TOG);
          #endif
     }
     if(tS->touch6)
     {
        #ifdef TOUCH_8_MODULE
        if(pS)
          changeRelayStatus(4, RLY_TOG);
        #endif
        
     }
     if(tS->touch7)
     {
          #ifdef TOUCH_8_MODULE
          if(pS)
            changeRelayStatus(2, RLY_TOG);
          #endif
          #ifdef TOUCH_4_MODULE
          changeRelayStatus(4, RLY_TOG);
          #endif
     }
     if(tS->touch9)
     {
          #ifdef TOUCH_8_MODULE
          if(pS)
            changeRelayStatus(1, RLY_TOG);
          #endif
          #ifdef TOUCH_4_MODULE
          changeRelayStatus(2, RLY_TOG);
          #endif
         #ifdef TOUCH_2_MODULE
         changeRelayStatus(4, RLY_TOG);
         #endif
     }
     if(tS->touch10)
     {
          #ifdef TOUCH_8_MODULE
          if(pS)
            changeRelayStatus(3, RLY_TOG);
          #endif
          #ifdef TOUCH_4_MODULE
          changeRelayStatus(1, RLY_TOG);
          #endif

          #ifdef TOUCH_2_MODULE_BELL
          changeRelayStatus(2, RLY_TOG);
          #endif
     }
     if(tS->touch11)
     {
        #ifdef TOUCH_8_MODULE
        changeMainPowerStatus(MAIN_PWR_TOGG);
        #endif
        
        #ifdef TOUCH_4_MODULE
          changeRelayStatus(3, RLY_TOG);
        #endif

        #ifdef TOUCH_2_MODULE
          changeRelayStatus(2, RLY_TOG);
        #endif
     }
     if(tS->touch12)
     {
          #ifdef TOUCH_8_MODULE
          if(pS)
          changeRelayStatus(6, RLY_TOG);
          #endif
     }
}

void IRAM_ATTR handleTouchInterrupt()
{
  Logger.Info("Touch Interrupt received");
  capInterruptCame=true;
}


void setup_touch()
{
  //Configure GPIO interrupt
  pinMode(CAP_INTR_PIN, INPUT);
  attachInterrupt(CAP_INTR_PIN, handleTouchInterrupt, RISING);
  //Enable Touch IC
  pinMode(ESP_CAP_RESET, OUTPUT);
  digitalWrite(ESP_CAP_RESET, LOW);
  delay(15); // 10mS delay needed before doin any I2C transactions on CAP1114
  //Initialise CAP
  boolean capStatus = bt_cap1114_init(CAP_I2CADDR);
  if(!capStatus)
  {  //try again
     digitalWrite(ESP_CAP_RESET, HIGH);
     delay(10);
     digitalWrite(ESP_CAP_RESET, LOW);
     delay(20);
     capStatus = bt_cap1114_init(CAP_I2CADDR);
     if(!capStatus)
     {
        Logger.Error("Touch initialisation failed");
        //update LED in such way that touch is failed to detect.
     }
  }
  //bt_cap1188_init(CAP_I2CADDR);
}

void loop_touch()
{
  if(capInterruptCame)
  {
     processTouchInterrupt(CAP_I2CADDR);
     capInterruptCame = false;
  }

  if(updateFanToggle)
  {
    changeFanStatus(1,FAN_CTRL_TOGGLE);
    updateFanToggle=false;
  }

  return;
}

void PowerkeyTimer()
{
   if(powerkeyIsinTouch)
   {
      Logger.Info("powerkeyTouchtime %d", powerkeyTouchtime);
      Logger.Info("In timer delta: %d", millis()-powerkeyTouchtime);

      powerkeyModeNo ++;
      if(powerkeyModeNo > SKIP_CONFIG_PRESS_TIME) 
      { 
        if (powerkeyUpdate == RESET_DEVICE) {
            staLedDetach();
        }   		
        powerkeyUpdate = RESTART_DEVICE;
        isConfigMode = false;
        if (hasConfigured) {
            staLedUpdated = setStaled(lastStatusLED);
            globalStatusLED = lastStatusLED;
        } else {
            staLedUpdated = setStaled(STA_NO_OPP_MODE);
            globalStatusLED = STA_NO_OPP_MODE;
        }
        FastLED.show();
      }
      else if(powerkeyModeNo > SPIFFS_RESET_PRESS_TIME) 
      {
          if (powerkeyUpdate == SMART_CONFIG) {
              staLedAttach(TICKER_SLOW, STA_RESET_MODE);
          }
          isConfigMode = true;
          powerkeyUpdate = RESET_DEVICE;
      } 
      else if(powerkeyModeNo > SMART_CONFIG_PRESS_TIME) 
      { // if it is more than 5 consider to goto config mode  
        if (!isConfigMode) {
            isConfigMode = true;
            staLedDetach();
            lastStatusLED = globalStatusLED;
            staLedUpdated = setStaled(STA_CONFIG_MODE);//update STA LED to Blue
            globalStatusLED = STA_CONFIG_MODE;
            FastLED.show();
        }
        powerkeyUpdate = SMART_CONFIG;
      }
   }
   else
   {
      powerTicker.detach();
      powerkeyUpdate = false;
   }
}

void fanDownkeyTimer()
{
   if(fanDownkeyIsinTouch)
   {
      fanDownkeyModeNo ++;
      if(fanDownkeyModeNo > FANDOWNKEY_PRESS_TIME) // if it is more than 5 consider to goto config mode
      {          
        fanDownkeyUpdate = true;
      }
   }
   else
   {
      fanDownTicker.detach();
      fanDownkeyUpdate = false;
   }
}

#endif
