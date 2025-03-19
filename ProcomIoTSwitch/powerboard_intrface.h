/*
 * relay_fan_data.h
 * Author: Venugopal reddy somu
 * Copyright @ Bridgethings pvt ltd.
 * All rights are reserved.
 * 
 * This file is a test file which can test the arduino power board.
 * The controling is via I2C interface.
 */
 

#ifndef __POWERBOARD_INTERFACE_H__
#define __POWERBOARD_INTERFACE_H__
#define ENABLE_POWERBOARD_STATUS_CHECK 1

enum { RLY_OFF=0, RLY_ON=1, RLY_TOG=2 };

enum FanNumber { FAN_1=0, FAN_2=1 };

enum FanMode{ FAN_OFF=0, FAN_LOW=1, FAN_SP_2=2, FAN_SP_3=3, FAN_SP_4=4, FAN_HIGH=5};  //FAN_LOW=1, FAN_MID=2, FAN_HIGH=3 };

struct relayStatusStruct
{
  uint8_t relay1:1;  //RELAY1 ON/OFF will store here
  uint8_t relay2:1;
  uint8_t relay3:1;
  uint8_t relay4:1;
  uint8_t relay5:1;
  uint8_t relay6:1;
  uint8_t relay7:1;
  uint8_t spare:1;
};

struct fanStatusStruct
{
  uint8_t fan_1:4;  // 2 bit info, it accpets mode 0(OFF),1(LOW_SPEED),2(MID_SPEED),3(HIGHSPEED) 
  uint8_t fan_2:4;  // 2 bit info, it accpets mode 0(OFF),1(LOW_SPEED),2(MID_SPEED),3(HIGHSPEED) 
  //uint8_t spare:4;  //spare 4 bit feilds
};

uint8_t powerBoradPowerOnStatus =0;
uint8_t grelayStatus=0;
uint8_t gfanStatus=0;

#define I2C_POWERBOARD_ADDR 8
#define I2C_REGISTER_RELAY 0x1
#define I2C_REGISTER_FAN 0x2
#define I2C_REGISTER_PWR 0x3
#define I2C_REGISTER_GET_RELAY_FAN_STATUS 0x4
#define I2C_REGISTER_PON_STATUS 0x5

//global variables
uint8_t grecRelayData =0;
uint8_t grecFanData=0;
uint8_t grecPwrData=0;

char sendStatusMsg[4][6] = {"None", "Relay", "Fan", "Power"};

void sendStatusToBoard(uint8_t addr, uint8_t data) {
    //uint8_t data =0x7;
    Wire.beginTransmission(I2C_POWERBOARD_ADDR); // transmit to device #8
    Wire.write(addr);        // sends register address
    Wire.write(data);              // sends one byte
    Wire.endTransmission();    // stop transmitting
    Logger.Info("%s value sent: %d", sendStatusMsg[addr], data);
    #ifdef MQTT_LOG_ENABLE
    logData = logData +"\n" + String(sendStatusMsg) + " value sent";
    #endif
}

void sendRelayData(uint8_t rdata)
{
    sendStatusToBoard(I2C_REGISTER_RELAY, rdata);
}

void sendfanData(uint8_t fdata)
{
    sendStatusToBoard(I2C_REGISTER_FAN, fdata);
}

void sendpwrData(uint8_t pdata)
{
    sendStatusToBoard(I2C_REGISTER_PWR, pdata);
}

uint16_t getponStatus()
{
  uint8_t ponStatus=0;
  Wire.beginTransmission(I2C_POWERBOARD_ADDR);
  Wire.write(I2C_REGISTER_PON_STATUS);    
  //Logger.Info("end status");// sends register address
  Wire.endTransmission();
  uint8_t numBytes =Wire.requestFrom(I2C_POWERBOARD_ADDR, 1);    // request 1 byte from slave device #8
  if(numBytes)
  {
    Logger.Info(numBytes);
    delay(1);  //casual delay insted of using while (Wire.available())
    ponStatus= Wire.read(); // receive a byte
    Logger.Info("PON Status: %d", ponStatus);         // print the character
  }
  else
  {
    ponStatus=0; 
  }

  powerBoradPowerOnStatus = ponStatus;
  return ponStatus;
}

void getrelayfanStatus()
{
  Wire.beginTransmission(I2C_POWERBOARD_ADDR); 
  Wire.write(I2C_REGISTER_GET_RELAY_FAN_STATUS);  // sends register address
  Wire.endTransmission();    // stop transmitting
  uint8_t numBytes = Wire.requestFrom(I2C_POWERBOARD_ADDR, 3);    // request 2 byte from slave device #8
  (void)numBytes;
  delay(1);  //casual delay insted of using while (Wire.available()) 
  grecRelayData = Wire.read(); // receive a byte
  grecFanData = Wire.read();   // receive second byte
  grecPwrData = Wire.read();   // receive second byte
  Logger.Info("Received relay: %d", grecRelayData);
  Logger.Info("Received Fan: %d", grecFanData); 
  Logger.Info("Received Pwr: %d", grecPwrData);
}

void powerboard_begin()
{
  //read the power board on satus
  if(getponStatus()!=1)
  {
    //reset the board
    //Reset the Arduino board via GPIO pull down
    digitalWrite(PWR_BRD_RESET_PIN, LOW);
    delay(10);
    digitalWrite(PWR_BRD_RESET_PIN, HIGH);
    //wiat till powerboard up and running
    Logger.Info("Waiting for Powerboard turn on..");
#if ENABLE_POWERBOARD_STATUS_CHECK
    while(!getponStatus())
    {
      Logger.print(".");
      delay(100);
    }
#endif
    Logger.println();
  }
  //Retrive the eeprom data and store into global variables.
  getrelayfanStatus();
  Logger.Info("Power board bootedup");
  //logData = logData +"\n" + "Power board bootedup";
}
#endif
