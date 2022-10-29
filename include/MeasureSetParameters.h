#pragma once
#include "CommonTypes.h"


typedef struct
{
   float    f_TDSValue;
   float    f_TempValue;
   float    f_Voltage;
   bool     b_PH_isValid;
   bool     b_EC_isValid;
   bool     b_Temp_isValid;
   bool     b_PH_LowFlag;
   bool     b_PH_HighFlag;
   bool     b_EC_LowFlag;
   bool     b_EC_HighFlag;
   bool     b_Temp_LowFlag;
   bool     b_Temp_HighFlag;   
   float    f_PHValue;
   float    f_ECValue;
   uint32_t u32_PHTimerCount;
   uint32_t u32_ECTimerCount;
   bool     waterPumpOnStatus;
   uint32_t waterPumpOnTime;
   uint32_t waterPumpOffTime;
   bool     lightPumpOnStatus;
   uint32_t lightPumpOnTime;
   uint32_t lightPumpOffTime;
   uint32_t PH_dosingPump_lowTime;
   uint32_t PH_dosingPump_highTime;
   uint32_t EC_dosingPump_low_ATime; 
   uint32_t EC_dosingPump_low_BTime;
   uint32_t EC_dosingPump_highTime;
   uint32_t waterLevelInCm;  
   uint32_t u32_CurrMeasureFreqInSec;     
   bool   manuallighton;
   bool   manualpumpon;
  // float waterLevelInCm;
}MeasurementParams_t;

typedef struct
{
  float    f_PH_LowThreshold = 5;  
  float    f_PH_HighThreshold = 6.5;
  float    f_EC_LowThreshold = 650;
  float    f_EC_HighThreshold = 900;
  float    f_Temp_LowThreshold = 5;
  float    f_Temp_HighThreshold = 30;
  uint32_t u32_LightsOnTime = 64800;
  uint32_t u32_LightsOffTime = 60;  
  uint32_t u32_PumpOnTime = 64800;  
  uint32_t u32_PumpOffTime = 60;  
  bool     isWaterLevelFull;
  float    f_PH_LowThresholdMin = 2;
  float    f_PH_LowThresholdMax = 14;
  float    f_EC_LowThresholdMin = 0;
  float    f_EC_HighThresholdMax = 2000;
  float    f_Temp_LowThresholdMin = 0;
  float    f_Temp_HighThresholdMax = 40;
  uint32_t u32_MeasureFreqInSec = 5;   ///measurement delay
  uint8_t  u8_DosageTurnOn = 1;
  uint32_t container_height = 20;
  uint32_t water_height = 20;
  uint8_t KitStart;
}setParameters_t;

void WriteSetParameters( setParameters_t setParams);
void ReadSetParameters( setParameters_t &setParams);
void ReadMeasureParameters( MeasurementParams_t setParams);
void LoadSetParameters(setParameters_t *pSetParams);
extern MeasurementParams_t measParams;
extern setParameters_t setParams;


