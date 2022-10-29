#pragma once
#include "CommonTypes.h"
#include "GPIOConfig.h"
#include "MeasureSetParameters.h"
//#include "Sensors.h"

namespace DosageSystemInterface
{

//mcp23017 config
#define PH_dosingPump_low 8
#define PH_dosingPump_high 9
#define EC_dosingPump_low_A 10
#define EC_dosingPump_low_B 11
#define EC_dosingPump_high 12

  class DosageSystem
  {
    Adafruit_MCP23017 mcp;
    MeasurementParams_t &measuredParams;
    setParameters_t &setParams;

    void ConfigureThreshold(setParameters_t *pSetParams);

    void pH_High_5_seconds();
    void pH_Low_5_seconds();
    void pH_Low_10_seconds();
    void pH_Low_15_seconds();
    void pH_Low_half_minute();
    void pH_Low_45_seconds();
    void pH_Low_one_minute();
    void pH_Low_two_minute();
    void pH_High_10_seconds();
    void pH_High_15_seconds();
    void pH_High_half_minute();
    void pH_High_45_seconds();
    void pH_High_one_minute();
    void pH_High_two_minute();
    void ec_Low_5_seconds();
    void ec_Low_10_seconds();
    void ec_Low_15_seconds();
    void ec_Low_one_minute();
    void ec_Low_two_minute();
    void ec_Low_45_seconds();
    void ec_Low_half_minute();
    void ec_High_5_seconds();
    void ec_High_10_seconds();
    void ec_High_15_seconds();
    void ec_High_one_minute();
    void ec_High_two_minute();
    void ec_High_45_seconds();
    void ec_High_half_minute();

  public:
    DosageSystem(MeasurementParams_t &measuredParamsTemp, setParameters_t &setParamsTemp)
        : measuredParams(measuredParamsTemp), setParams(setParamsTemp)
    {
    }
    void Setup();
    bool Process();
    void ClearTimers();
    void IRAM_ATTR onTimer();
    void ValidateMeasuredReadingsTDS(void);
    void ValidateMeasuredReadingsPH(void);
    void ValidateMeasuredReadingsTEMP(void);
    void tdsLow();
    void tdsHigh();
    void tdsOptimum();
    void phLow();
    void phHigh();  
    void phOptimum();
  };

}