#ifndef __BT_CAP1114_H__
#define __BT_CAP1114_H__

#include "SerialLogger.h"

#define I2C_DEBUG 1

//Cap I2C registers.
#define CAP1114_PRODID 0xFD
#define CAP1114_MANUID 0xFE
#define CAP1114_REV 0xFF
#define CAP1114_BTNSTATUS1 0x3
#define CAP1114_BTNSTATUS2 0x4
#define CAP1114_CFG2 0x40
#define CAP1114_SENSITVITY_CTRL 0x1F
#define CAP1114_SENSOR_INPUT_CFG2 0x23
#define CAP1114_MTBLK 0x2A
#define CAP1114_MAIN 0x00
#define CAP1114_MAIN_INT 0x01

#define CAP1114_BTN_CFG 0x22
//shared API's
boolean bt_cap1114_init(uint8_t i2c_addr);
void clearCap1114Interrupt(uint8_t i2c_addr);
uint16_t readcap1114TouchData(uint8_t i2c_addr);

//internal function call's
uint8_t readRegister(uint8_t i2caddr, uint8_t reg);
void writeRegister(uint8_t i2caddr, uint8_t reg, uint8_t value);


//----- Default Arduino I2C Lib-----
//If any I2C fail on read or write, check the code below

static uint8_t i2cread(void) {
  #if ARDUINO >= 100
  return Wire.read();
  #else
  return Wire.receive();
  #endif
}

static void i2cwrite(uint8_t x) {
  #if ARDUINO >= 100
  Wire.write((uint8_t)x);
  #else
  Wire.send(x);
  #endif
}
//------ Arduino I2C lib End

uint8_t readRegister(uint8_t i2caddr, uint8_t reg)
{
    Wire.beginTransmission(i2caddr);
    i2cwrite(reg);
    Wire.endTransmission();
    Wire.requestFrom((int)i2caddr, 1);
    return (i2cread());
}

void writeRegister(uint8_t i2caddr, uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(i2caddr);
    i2cwrite((uint8_t)reg);
    i2cwrite((uint8_t)(value));
    Wire.endTransmission();
}

boolean bt_cap1114_init(uint8_t cap_i2caddr)
{
  #ifdef I2C_DEBUG
  Logger.Info("Product ID: 0x%X", readRegister(cap_i2caddr, CAP1114_PRODID));
  Logger.Info("Manuf. ID: 0x%X", readRegister(cap_i2caddr, CAP1114_MANUID));
  Logger.Info("Revision: 0x%X", readRegister(cap_i2caddr, CAP1114_REV));
  
  if ( (readRegister(cap_i2caddr, CAP1114_PRODID) != 0x3A) ||
       (readRegister(cap_i2caddr,CAP1114_MANUID) != 0x5D) ||
       (readRegister(cap_i2caddr,CAP1114_REV) != 0x80)) {
    Logger.Error("BT CAP1114 init failed");
    return false;
  }
  else
  {
    Logger.Info("BT CAP1114 init success");
  }
  #endif

  //Ungroup touchpads and enable release interrupt
  writeRegister(cap_i2caddr, CAP1114_CFG2, 0x02);

  //Sensitivity setting
  writeRegister(cap_i2caddr, CAP1114_SENSITVITY_CTRL, 0x1F);
  
  //Controls the MPRESS control for allsensor inputs
  writeRegister(cap_i2caddr, CAP1114_SENSOR_INPUT_CFG2, 0x0F);

  // allow multiple touch blocking and set to 1 only
  writeRegister(cap_i2caddr, CAP1114_MTBLK, 0x83); 
  
  // Time interwel to recalibrate the button -> set to 11.2Sec
  writeRegister(cap_i2caddr, CAP1114_BTN_CFG, 0xF4); 

  //Clear touch interrupt flag register
  clearCap1114Interrupt(cap_i2caddr);

  return true;
}


uint16_t readcap1114TouchData(uint8_t i2c_addr)
{
  uint16_t t=0;
  uint8_t t1 = readRegister(i2c_addr, CAP1114_BTNSTATUS1);
  uint8_t t2 = readRegister(i2c_addr, CAP1114_BTNSTATUS2);
  //Logger.Info("Touch data t1: %d ",t1);
  //Logger.Info("Touch data t2: %d \n",t2);
  //mask the not used touch data
  t1 = t1 & 0x3F;
  t2 = t2 & 0x3F;
  t = (t2<<8) | t1;

  Logger.Info("Touch data: %d",t);
  if (!t) {
    Logger.Info("No data");
    return t;
  }

  return t;
}

void clearCap1114Interrupt(uint8_t i2c_addr)
{
   writeRegister(i2c_addr, CAP1114_MAIN, readRegister(i2c_addr,CAP1114_MAIN) & ~CAP1114_MAIN_INT); 
}


#endif