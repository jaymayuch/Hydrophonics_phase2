#include "Sensors.h"
#include "GPIOConfig.h"

#define ESPADC 4096.0
#define ESPVOLTAGE 3300

namespace SensorInterface
{
    //Should be in global scope
    const uint8_t oneWireBus = TEMPSENSORPIN;
    OneWire oneWire(oneWireBus);
    DallasTemperature stempSens(&oneWire);

    void Sensors::Init(void)
    {
        for (uint8_t sensor = 0; sensor < eNUM_SENSORS; sensor++)
        {
            switch (sensor)
            {
            case ePH_SENSOR:
                sPHSensor.begin();
                break;
            case eEC_SENSOR:
                sECSensor.begin();
                break;
            case eTEMP_SENSOR:
                stempSens.begin();
                break;
            }
        }
    }

    void Sensors::getSensorValues(eSensorTypes sensor, float &f_measuredValue)
    {
        float f_Voltage = 0.0;
        float f_Temp = 0.0;
        float f_MeasuredTemp = 0.0;

        switch (sensor)
        {
        case ePH_SENSOR:
           // stempSens.requestTemperatures();
           // f_MeasuredTemp = stempSens.getTempCByIndex(0);

            Serial.println("PH Raw");
            Serial.println(analogRead(PH_PIN));

            f_Voltage = analogRead(PH_PIN) / ESPADC * ESPVOLTAGE; // read the voltage
            //f_measuredValue = (sPHSensor.readPH(f_Voltage, f_MeasuredTemp)) * 10;
            f_measuredValue = (sPHSensor.readPH(f_Voltage, 25)) * 10;
            f_measuredValue = f_measuredValue / 10;            
            sPHSensor.calibration(f_Voltage, 25); //TBD
            break;
        case eEC_SENSOR:

           // stempSens.requestTemperatures();
           // f_MeasuredTemp = stempSens.getTempCByIndex(0);

            f_Voltage = analogRead(EC_PIN) / ESPADC * ESPVOLTAGE; // read the voltage
            Serial.println("EC");
            Serial.println(f_Voltage);
           // f_measuredValue = (sECSensor.readEC(f_Voltage, f_MeasuredTemp)) * 100;            
          //  sECSensor.calibration(f_Voltage, f_MeasuredTemp); //TBD
             f_measuredValue = ((sECSensor.readEC(f_Voltage, 25))*156); 
             Serial.println("EC measured");
             Serial.println(f_measuredValue);
             sECSensor.calibration(f_Voltage, 25); //TBD
            break;
        case eTEMP_SENSOR:
            stempSens.requestTemperatures();
            f_measuredValue = stempSens.getTempCByIndex(0);
            Serial.println("Temp");
            Serial.println(f_measuredValue);
            break;
        case eNUM_SENSORS:
            break;
        }
    }

    void Sensors::calibrateSensors(eSensorTypes sensor, float f_Voltage, float f_Temp)
    {
        switch (sensor)
        {
        case ePH_SENSOR:
            break;
        case eEC_SENSOR:
            break;
        case eTEMP_SENSOR:
            break;
        case eNUM_SENSORS:
            break;
        }
    }

    enum hydroProcessStates
    {
/////////***** TDS Dosing will start on 1st reading ******//////        
        eInitState = 0,
        eReadTempSens,      
        eTurnOnTDSSensor,
        eReadTDSSensor,
        eTurnOffTDSSensor,
        eTurnOnPHSensor,
        eReadPHSensor,
        eTurnOffPHSensor,
        eValidateMeasurementTDS,
        eDosageProcess,
        eDelayBetweenMeas,
////////*****PH Dosing will start on 2nd readings ****//////////
        eReadTempSens2,
        eTurnOnTDSSensor2,
        eReadTDSSensor2,
        eTurnOffTDSSensor2,
        eTurnOnPHSensor2,
        eReadPHSensor2,
        eTurnOffPHSensor2,      
        eValidateMeasurementPH,
        eDosageProcess2,
        eDelayBetweenMeas2,

        eTurnOnWaterPump,
        eTurnOffWaterPump,
        eTurnOnLight,
        eTurnOffLight,
        eWaterPumpInit,
        eCalibrationState,
        eNumOfStates
    };
    typedef struct
    {
        hydroProcessStates state;
        uint32_t timeCountInSec;
        uint32_t currtimeCountInSec;
    } hydroProcessStates_t;

    hydroProcessStates_t processStateCtx[eNumOfStates] =
        {
            //State                  TimeCount(sec)      Current Count(sec)
            {eInitState, 5, 0},
            {eReadTempSens, 5, 0},     
            {eTurnOnTDSSensor, 5, 0},
            {eReadTDSSensor, 30, 0},
            {eTurnOffTDSSensor, 5, 0},
            {eTurnOnPHSensor, 5, 0},
            {eReadPHSensor, 30, 0},
            {eTurnOffPHSensor, 5, 0},
            {eValidateMeasurementTDS, 1, 0},
            {eDosageProcess, 1, 0},
            {eDelayBetweenMeas, 300, 0}, //5 min delay for the TDS dosing mixing

            {eReadTempSens2, 5, 0},          
            {eTurnOnTDSSensor2, 5, 0},
            {eReadTDSSensor2, 30, 0},
            {eTurnOffTDSSensor2, 5, 0},
            {eTurnOnPHSensor2, 5, 0},
            {eReadPHSensor2, 30, 0},
            {eTurnOffPHSensor2, 5, 0},

            {eValidateMeasurementPH, 1, 0},
            {eDosageProcess2, 1, 0},
            {eDelayBetweenMeas2, 900, 0},//15 min delay for the PH dosing and entire

            {eTurnOnWaterPump, 10, 0},
            {eTurnOffWaterPump, 10, 0},
            {eTurnOnLight, 10, 0},
            {eTurnOffLight, 10, 0},
            {eWaterPumpInit, 1, 0},
            {eCalibrationState, 2, 0}};

    hydroProcessStates state = eInitState;
    hydroProcessStates waterPumpstate = eTurnOnWaterPump; //at begining of kit water pump will on  //eTurnOffWaterPump;
    hydroProcessStates lightstate = eTurnOnLight; //eTurnOffLight;
    

    void setStateTimerinSec(hydroProcessStates &state, uint32_t timeInSec)
    {
        processStateCtx[state].timeCountInSec = timeInSec;
    }
    void checkTimerForNextState(hydroProcessStates &state, hydroProcessStates currentState, hydroProcessStates nextState)
    {
        if (processStateCtx[currentState].currtimeCountInSec == 0)
        {
            processStateCtx[currentState].currtimeCountInSec = processStateCtx[currentState].timeCountInSec;
        }
        if (processStateCtx[currentState].currtimeCountInSec != 0)
        {
            processStateCtx[currentState].currtimeCountInSec--;
            if (processStateCtx[currentState].currtimeCountInSec == 0)
            {
                state = nextState;
            }
        }
    }

    uint32_t getElapsedTimeInSec(hydroProcessStates state)
    {
        return ((processStateCtx[state].timeCountInSec) - (processStateCtx[state].currtimeCountInSec));
    }

    void ctrlWaterPump(void)
    {
        if (waterPumpstate == eTurnOffWaterPump)
        {            
            //Pump Off
            digitalWrite(WATER_PUMP, HIGH);
            setStateTimerinSec(waterPumpstate, setParams.u32_PumpOffTime);
            measParams.waterPumpOnStatus = LOW;
            checkTimerForNextState(waterPumpstate, eTurnOffWaterPump, eTurnOnWaterPump);
            measParams.waterPumpOffTime = getElapsedTimeInSec(eTurnOffWaterPump);
            Serial.println("**********Water Pump OFF **********");
            Serial.print("Pump Off time=");
            Serial.print(measParams.waterPumpOffTime);
            Serial.println();
        }
        else
        {
            Serial.println("eTurnOnWaterPump");
            digitalWrite(WATER_PUMP, LOW);
            setStateTimerinSec(waterPumpstate, setParams.u32_PumpOnTime);
            measParams.waterPumpOnStatus = HIGH;
            checkTimerForNextState(waterPumpstate, eTurnOnWaterPump, eTurnOffWaterPump);
            measParams.waterPumpOnTime = getElapsedTimeInSec(eTurnOnWaterPump);
            Serial.println("**********Water Pump ON **********");
            Serial.print("Pump On time=");
            Serial.print(measParams.waterPumpOnTime);
            Serial.println();
        }
    }
    void Sensors::controlLights(void)
    {
        //Light On off to be aded
        if (lightstate == eTurnOffLight)
        {
            Serial.println("eTurnOffLight");
            digitalWrite(AC2, HIGH);
            setStateTimerinSec(lightstate, setParams.u32_LightsOffTime);
            measParams.lightPumpOnStatus = LOW;
            //measParams.lightPumpOffTime = getElapsedTimeInSec(eTurnOffLight);
            measParams.lightPumpOnTime = 0;
            checkTimerForNextState(lightstate, eTurnOffLight, eTurnOnLight);
            measParams.lightPumpOffTime = getElapsedTimeInSec(eTurnOffLight);
            Serial.println("**********Light Off **********");
            Serial.print("Light Off time=");
            Serial.print(measParams.lightPumpOffTime);
            Serial.println();
        }
        else
        {            
            setStateTimerinSec(lightstate, setParams.u32_LightsOnTime);
            digitalWrite(AC2, LOW);
            measParams.lightPumpOnStatus = HIGH;
            measParams.lightPumpOffTime = 0;
            checkTimerForNextState(lightstate, eTurnOnLight, eTurnOffLight);
            measParams.lightPumpOnTime = getElapsedTimeInSec(eTurnOnLight);
            Serial.println("**********Light ON **********");
            Serial.print("Light On time=");
            Serial.print(measParams.lightPumpOnTime);
            Serial.println();
        }
    }
    void Sensors::ManualcontrolLights(bool on)
    {
        if(on==true)
        {
            if(lightstate!=eTurnOnLight)
            {
                processStateCtx[lightstate].currtimeCountInSec = 0;
                lightstate = eTurnOnLight;            
                Serial.print("Manually Light turned On");
                Serial.println();
            }
        }
        else
        {            
            if(lightstate!=eTurnOffLight)         
            {
                processStateCtx[lightstate].currtimeCountInSec = 0;
                lightstate = eTurnOffLight;   
                Serial.print("Manually Light turned Off");
                Serial.println();
            }                        
        }
    }

    void Sensors::ManualcontrolPump(bool on)
    {
        if(on==true)
        {
            if(waterPumpstate!=eTurnOnWaterPump)
            {
                processStateCtx[lightstate].currtimeCountInSec = 0;
                waterPumpstate = eTurnOnWaterPump;            
                Serial.print("Manually Water Pump turned On");
                Serial.println();
            }
        }
        else
        {            
            if(waterPumpstate!=eTurnOffWaterPump)         
            {
                processStateCtx[lightstate].currtimeCountInSec = 0;
                waterPumpstate = eTurnOffWaterPump;   
                Serial.print("Manually Water pump turned Off");
                Serial.println();
            }                        
        }
    }

    void Sensors::MeasurementProcess(void)
    {
        static float debugCount;
        static uint32_t u32_MeasureFreqInSec = 0;
        Serial.print("Time=");
        Serial.print(++debugCount);
        Serial.println();
#ifdef CALIBRATION
    state = eCalibrationState;
#endif        
        switch (state)
        {
        case eInitState:
        {
            if( setParams.KitStart == 1)
            {
            Serial.println("**********Initialization State***************");
#ifndef DEMO_MODE            
            checkTimerForNextState(state, eInitState, eReadTempSens);
#endif            
            }
            else
            {                
                checkTimerForNextState(state, eInitState, eInitState);
            }
        }
        break;
        case eReadTempSens:
        {
            Serial.println("**********Reading Temperature State***********");
            getSensorValues(eTEMP_SENSOR, measuredParams.f_TempValue);
            if( measParams.u32_CurrMeasureFreqInSec++ >= setParams.u32_MeasureFreqInSec )
            {
                checkTimerForNextState(state, eReadTempSens, eTurnOnTDSSensor);
                measParams.u32_CurrMeasureFreqInSec = 0;
            }
        }
        break;
        case eTurnOnTDSSensor:
        {
            Serial.println("**********Turn ON TDS Sensor State***********");    
            Serial.println("eTurnOnTDSSensor");
            digitalWrite(TS_SUPPLY_VCC, LOW);
            digitalWrite(TS_SUPPLY_GND, HIGH);
            checkTimerForNextState(state, eTurnOnTDSSensor, eReadTDSSensor);
        }
        break;
        case eReadTDSSensor:
        {
            Serial.println("**********Read TDS Sensor State***********");                
            getSensorValues(eEC_SENSOR, measuredParams.f_ECValue);
            checkTimerForNextState(state, eReadTDSSensor, eTurnOffTDSSensor);
        }
        break;
        case eTurnOffTDSSensor:
        {
            Serial.println("**********Turn Off TDS Sensor State**********");    
            digitalWrite(TS_SUPPLY_VCC, HIGH);
            digitalWrite(TS_SUPPLY_GND, LOW);
            checkTimerForNextState(state, eTurnOffTDSSensor, eTurnOnPHSensor);
        }
        case eTurnOnPHSensor:
        {
            Serial.println("**********Turn ON PH Sensor State***********");            
            checkTimerForNextState(state, eTurnOnPHSensor, eReadPHSensor);
            digitalWrite(PH_SUPPLY_VCC, LOW);
            digitalWrite(PH_SUPPLY_GND, HIGH);
        }
        break;
        case eReadPHSensor:
        {
            Serial.println("**********Read PH Sensor ADC State***********");
            getSensorValues(ePH_SENSOR, measuredParams.f_PHValue);
            checkTimerForNextState(state, eReadPHSensor, eTurnOffPHSensor);
        }
        break;
        case eTurnOffPHSensor:
        {
            Serial.println("**********Turn Off PH Sensor State***********");            
            checkTimerForNextState(state, eTurnOffPHSensor, eValidateMeasurementTDS);
            digitalWrite(PH_SUPPLY_VCC, HIGH);
            digitalWrite(PH_SUPPLY_GND, LOW);
        }
         case eValidateMeasurementTDS:
        {
            Serial.println("**********Validate MeasuerementTDS**********");            
            dosageSystem.ValidateMeasuredReadingsTDS();
            if( setParams.u8_DosageTurnOn == 1)
            {
                checkTimerForNextState(state, eValidateMeasurementTDS, eDosageProcess);
            }
            else
            {
                checkTimerForNextState(state, eValidateMeasurementTDS, eDelayBetweenMeas);                
            }
            dosageSystem.ClearTimers();
        }
        break;

        case eDosageProcess:
        {
            Serial.println("**********Process Dosage TDS**********");
            if (dosageSystem.Process() == true )
            checkTimerForNextState(state, eDosageProcess, eDelayBetweenMeas);
        }
        break;
        case eDelayBetweenMeas:
        {
            Serial.println("**********Delay between Measuerement TDS**********");
            checkTimerForNextState(state, eDelayBetweenMeas, eReadTempSens2);
        }
        break;
        case eReadTempSens2:
        {
            Serial.println("**********Reading Temperature State***********");
            getSensorValues(eTEMP_SENSOR, measuredParams.f_TempValue);
            checkTimerForNextState(state, eReadTempSens2, eTurnOnTDSSensor2);
        }
        break;
        case eTurnOnTDSSensor2:
        {
            Serial.println("**********Turn ON TDS Sensor State***********");    
            Serial.println("eTurnOnTDSSensor");
            digitalWrite(TS_SUPPLY_VCC, LOW);
            digitalWrite(TS_SUPPLY_GND, HIGH);
            checkTimerForNextState(state, eTurnOnTDSSensor2, eReadTDSSensor2);
        }
        break;
        case eReadTDSSensor2:
        {
            Serial.println("**********Read TDS Sensor State***********");                
            getSensorValues(eEC_SENSOR, measuredParams.f_ECValue);
            checkTimerForNextState(state, eReadTDSSensor2, eTurnOffTDSSensor2);
        }
        break;
        case eTurnOffTDSSensor2:
        {
            Serial.println("**********Turn Off TDS Sensor State**********");    
            digitalWrite(TS_SUPPLY_VCC, HIGH);
            digitalWrite(TS_SUPPLY_GND, LOW);
            checkTimerForNextState(state, eTurnOffTDSSensor2, eTurnOnPHSensor2);
        }
        break;

        case eTurnOnPHSensor2:
        {
            Serial.println("**********Turn ON PH Sensor State***********");            
            checkTimerForNextState(state, eTurnOnPHSensor2, eReadPHSensor2);
            digitalWrite(PH_SUPPLY_VCC, LOW);
            digitalWrite(PH_SUPPLY_GND, HIGH);
        }
        break;
        case eReadPHSensor2:
        {
            Serial.println("**********Read PH Sensor ADC State***********");
            getSensorValues(ePH_SENSOR, measuredParams.f_PHValue);
            checkTimerForNextState(state, eReadPHSensor2, eTurnOffPHSensor2);
        }
        break;
        case eTurnOffPHSensor2:
        {
            Serial.println("**********Turn Off PH Sensor State***********");            
            checkTimerForNextState(state, eTurnOffPHSensor2, eValidateMeasurementPH);
            digitalWrite(PH_SUPPLY_VCC, HIGH);
            digitalWrite(PH_SUPPLY_GND, LOW);
        }
        case eValidateMeasurementPH:
        {
            Serial.println("**********Validate Measuerement PH**********");            
            dosageSystem.ValidateMeasuredReadingsPH();
            if( setParams.u8_DosageTurnOn == 1)
            {
                checkTimerForNextState(state, eValidateMeasurementPH, eDosageProcess2);
            }
            else
            {
                checkTimerForNextState(state, eValidateMeasurementPH, eDelayBetweenMeas2);
            }
            
            dosageSystem.ClearTimers();
        }
        break;
        case eDosageProcess2:
        {
            Serial.println("**********Process Dosage PH**********");
            if (dosageSystem.Process() == true )
            checkTimerForNextState(state, eDosageProcess2, eDelayBetweenMeas2);
        }
        break;
        case eDelayBetweenMeas2:
        {
            Serial.println("**********Delay between Measuerement 2**********");
            checkTimerForNextState(state, eDelayBetweenMeas2, eReadTempSens);
        }
        break;
        case eCalibrationState:
            digitalWrite(TS_SUPPLY_VCC, LOW);
            digitalWrite(TS_SUPPLY_GND, HIGH);

            digitalWrite(PH_SUPPLY_VCC, LOW);
            digitalWrite(PH_SUPPLY_GND, HIGH);
            getSensorValues(eEC_SENSOR, measuredParams.f_ECValue);
            getSensorValues(ePH_SENSOR, measuredParams.f_PHValue);

            break;
        }
        if( state!=eCalibrationState )
        {
#ifdef DEMO_MODE               
           // If not on manual control then use schedule timers
            if(measParams.manualpumpon == true)
            {
                ManualcontrolPump(true);
            }
            else 
            {
                ManualcontrolPump(false);                
            } 
#endif
           ctrlWaterPump();
#ifdef DEMO_MODE                 
            // If not on manual control then use schedule timers
            if(measParams.manuallighton == true)
            {
                ManualcontrolLights(true);
            }
            else 
            {
                ManualcontrolLights(false);                

            }            
#endif
            controlLights();
           getWaterLevelinCm();
        }
    }
    
    long microsecondsToInches(long microseconds)
    {
        return microseconds / 74 / 2;
    }

    long microsecondsToCentimeters(long microseconds)
    {
        return microseconds / 29 / 2;
    }
    long Sensors::getWaterLevelinCm()
    {
        long duration, inches, cm;
        pinMode(ULTRA_TRIG_PIN, OUTPUT);
        digitalWrite(ULTRA_TRIG_PIN, LOW);
        delayMicroseconds(2);
        digitalWrite(ULTRA_TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(ULTRA_TRIG_PIN, LOW);
        pinMode(ULTRA_TRIG_PIN, INPUT);
        duration = pulseIn(ULTRA_ECHO_PIN, HIGH);
        inches = microsecondsToInches(duration);
        cm = microsecondsToCentimeters(duration);
        Serial.print("Water Level=");
        Serial.print(inches);
        Serial.print("inch, ");
        Serial.print(cm);
        Serial.print("cm");
        Serial.println();
        //delay(100);
        float water = setParams.water_height - cm;
        float water1 = water / 40;
        //Serial.print("divided");
        //Serial.println(water1);
        measuredParams.waterLevelInCm = water1 * 100 ; //He-cm/Height
        //Serial.print("bucket height -");
        //Serial.println((float)((40 - cm) / 20));

        Serial.print("Measured cm % in bucket = ");
        Serial.println(measuredParams.waterLevelInCm);
        return measuredParams.waterLevelInCm;
    }
}