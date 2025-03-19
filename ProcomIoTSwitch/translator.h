#ifndef __TRANSLATE_H__
#define __TRANSLATE_H__

#include <ArduinoJson.h>

void toyamaToProcom(int switchNum, int percent) {
    switch (switchNum) {
        case 0:
            pS=percent;
            changeMainPowerStatus(pS);
            break;

        case 1:
            rS.relay1 = percent;
            break;
        
        case 2:
            rS.relay2 = percent;
            break;
        
        case 3:
            rS.relay3 = percent;
            break;
        
        case 4:
            rS.relay4 = percent;
            break;
        
        case 5:
            rS.relay5 = percent;
            break;
        
        case 6:
            rS.relay6 = percent;
            break;
        
        case 7:
            rS.relay7 = percent;
            break;
        
        case 9:
        case 10:
            if(percent == 0 )//|| fanCloudData==1) //command is related to FAN ON/OFF
            {
                changeFanStatus(switchNum - 8, percent);  //fanData 0 -> OFF, fanData 1 -> restore to previous status
            } else  //command have 5 fan speed 3,4,5,7,8
            {
                setFanSpeed(switchNum - 8, percent);                
            }
            break;
        
        default:
            break;
    }

    gUpdateRelay = true;
    gUpdateCloud = true;
}

#endif