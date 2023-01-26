#pragma once
//#include "SensorApi.h"
#include "MeasureSetParameters.h"
#include "DosageSystem.h"

namespace SensorInterface
{
    typedef enum sensorTypes_t
    {
        ePH_SENSOR = 0,
        eEC_SENSOR,
        eTEMP_SENSOR,
        eNUM_SENSORS
    }eSensorTypes;

    class Sensors
    {
        DFRobot_ESP_PH_WITH_ADC sPHSensor;
        //DFRobot_ESP_PH          sPHSensor;
        DFRobot_ESP_EC          sECSensor;
        uint8_t                 u8_numOfSensors;
        setParameters_t         &setParams;
        MeasurementParams_t     &measuredParams;
        DosageSystemInterface::DosageSystem            &dosageSystem;

    public:
        Sensors(uint8_t u8_numOfSensors, setParameters_t &setParamstemp, MeasurementParams_t &measuredParamsTemp,DosageSystemInterface::DosageSystem &dosageSysemTemp )
            : u8_numOfSensors(u8_numOfSensors), setParams(setParamstemp), measuredParams(measuredParamsTemp),dosageSystem(dosageSysemTemp)
        {
        }
        void Init();
        void getSensorValues(eSensorTypes sensor, float &f_measuredValue);        
        void calibrateSensors(eSensorTypes sensor, float f_Voltage, float f_Temp);
        void MeasurementProcess(void);
        void controlLights();
        long getWaterLevelinCm();
    };
}