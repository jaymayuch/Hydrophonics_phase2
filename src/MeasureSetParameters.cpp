#include "GPIOConfig.h"
#include "MeasureSetParameters.h"
#include "CommonTypes.h"
//

extern EEPROMClass EEPROM;
setParameters_t    setParams;
MeasurementParams_t measParams;

void WriteSetParameters(setParameters_t *pSetParams)
{
    EEPROM.writeBytes(SET_PARAMS_ADDRESS,pSetParams,sizeof(setParams) );
    EEPROM.commit();
    memcpy(&setParams, pSetParams, sizeof(setParams));
}

void LoadSetParameters(setParameters_t *pSetParams)
{
    int isignatRead = 0;
    int configVersion;
    isignatRead = EEPROM.readInt((int)SIGNATURE_ADDRESS);
    configVersion = EEPROM.readInt((int)CONFIG_VERSION_ADDRESS);
    //First time fresh board
    if(0)//if ((isignatRead != SIGNATURE)||(configVersion!=CONFIG_VERSION))
    {
        EEPROM.writeBytes(SET_PARAMS_ADDRESS,&setParams,sizeof(setParams) );
        EEPROM.writeInt((int) SIGNATURE_ADDRESS, SIGNATURE);
        EEPROM.writeInt((int) CONFIG_VERSION_ADDRESS, CONFIG_VERSION);
        EEPROM.commit();
    }
    else
    {
        //EEPROM.readBytes(SET_PARAMS_ADDRESS,pSetParams,sizeof(setParams) );
    }
    Serial.print("configversion=");
    Serial.println(configVersion);
    
}

void ReadSetParameters(setParameters_t *pSetParams)
{
    EEPROM.readBytes(SET_PARAMS_ADDRESS,pSetParams,sizeof(setParams) );
    memcpy(pSetParams, &setParams, sizeof(setParams));
}

void ReadMesParameters(MeasurementParams_t *pSetParams)
{
    memcpy(pSetParams, &measParams, sizeof(measParams));
}
