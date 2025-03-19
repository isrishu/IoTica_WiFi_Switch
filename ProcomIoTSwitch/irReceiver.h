#include <IRRecv.h>
#include "SerialLogger.h"

//List of IR key codes: in NEC
#define IR_PWR 0xf714eb  //0x1fe48b7
#define IR_MODE 0x1FE48B7 
#define IR_1 0xf715ea  //0x1fe50af
#define IR_2 0xf716e9  //0x1fed827
#define IR_3 0xf70ef1  //0x1fef807
#define IR_4 0xf70df2  //0x1fe30cf
#define IR_5 0xf70cf3  //0x1feb04f
#define IR_6 0xf701fe  //0x1fe708f
#define IR_7 0xf700ff  //0x1fe00ff
#define IR_FAN_1 0x1fef00f  //0x1fef00f
#define IR_FAN_2 0x1fe9867  //0x1fe9867
//#define IR_0 0x1fee01f
#define IR_VOL_P 0xf708f7  //0x1fe609f
#define IR_VOL_N 0xf70af5  //0x1fea05f
#define IR_PRG_P 0xef20df  //0x1fec03f
#define IR_PRG_N 0xef609f  //0x1fe40bf
#define IR_PLAY 0x1fe807f
#define IR_OK 0x1fe7887   
#define IR_RPT 0x1fe10ef

IRRecv remote1(IR_RMT_CHANNEL);  //using RMT channel 6 for IR remote

void setup_IrRecever()
{
   remote1.start(RECV_PIN);
}

void loop_IrReceiver()
{
  if(remote1.available())
  {
    char* rcvGroup;
    uint32_t result = remote1.read(rcvGroup);
    if (result) {
        Logger.Info("Received: %s/0x%x", rcvGroup, result);
    }
    switch(result)
    {
      case IR_PWR:
             #ifdef TOUCH_8_MODULE
             changeMainPowerStatus(MAIN_PWR_TOGG);
             #endif
             break;
      case IR_1:
              if(pS)
              changeRelayStatus(1, RLY_TOG);
              break;
      case IR_2:
              if(pS)
              changeRelayStatus(2, RLY_TOG);
              break;
      case IR_3:
              if(pS)
              changeRelayStatus(3, RLY_TOG);
              break;
      case IR_4:
              if(pS)
              changeRelayStatus(4, RLY_TOG);
              break; 
      case IR_5:
              if(pS)
              changeRelayStatus(5, RLY_TOG);
              break;
      case IR_6:
              if(pS)
              changeRelayStatus(6, RLY_TOG);
              break;
      case IR_7:
              if(pS)
              changeRelayStatus(7, RLY_TOG);
              break;
      case IR_FAN_1:
              //New requirement:Disable FAN ON/OFF from remote
              //if(pS)
              //changeFanStatus(1,FAN_CTRL_TOGGLE);
              break;
      case IR_FAN_2:
              //New requirement:Disable FAN ON/OFF from remote
              //if(pS)
              //changeFanStatus(2,FAN_CTRL_TOGGLE); 
              break;
      case IR_VOL_P:
              if(pS)
                  changeFanMode(1, FAN_CTRL_UP);
              break;
      case IR_VOL_N:
              if(pS)
                  changeFanMode(1, FAN_CTRL_DOWN);
              break;
      case IR_PRG_P:
              if(pS)
                  changeFanMode(2, FAN_CTRL_UP);
              break;
      case IR_PRG_N:
              if(pS)
                  changeFanMode(2, FAN_CTRL_DOWN);
              break;
      default:              
              break;
      
    }     
  }
  return;
}
