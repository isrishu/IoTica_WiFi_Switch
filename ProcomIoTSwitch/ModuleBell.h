#ifndef __MODULE_BELL_H__
#define __MODULE_BELL_H__
#include "powerboard_intrface.h"
#include "procom_global.h"
#include "bt_touch_interface.h"

float bellTickerPeriod=0.15;

void bellTimer()
{
  changeRelayStatus(1, RLY_OFF);
  powerTicker.detach();
}

void triggerCallingBell()
{
    changeRelayStatus(1, RLY_ON);
    powerTicker.attach(bellTickerPeriod, bellTimer);
}

#endif