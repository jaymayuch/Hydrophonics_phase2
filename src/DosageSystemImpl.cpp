#include "DosageSystem.h"
#include "MeasureSetParameters.h"

namespace DosageSystemInterface
{
    void IRAM_ATTR DosageSystem::onTimer()
    {
        measuredParams.u32_PHTimerCount++;
        measuredParams.u32_ECTimerCount++;
    }

    void DosageSystem::Setup()
    {
        Wire.begin(MCP_SDA_PIN, MCP_SCL_PIN);     //ESP-32 (SDA,SCL) for mcp23017 IO expander pins
        mcp.begin();                              // Default device address 0
        mcp.pinMode(PH_dosingPump_low, OUTPUT);   // ph dosing low pump
        mcp.pinMode(PH_dosingPump_high, OUTPUT);  // ph dosing high pump
        mcp.pinMode(EC_dosingPump_low_A, OUTPUT); // ec dosing low pump
        mcp.pinMode(EC_dosingPump_low_B, OUTPUT); // ec dosing low pump
        mcp.pinMode(EC_dosingPump_high, OUTPUT);  // ec dosing high pump

        mcp.digitalWrite(PH_dosingPump_low, HIGH);   // low triggered relay
        mcp.digitalWrite(PH_dosingPump_high, HIGH);  // low triggered relay
        mcp.digitalWrite(EC_dosingPump_low_A, HIGH); // low triggered relay
        mcp.digitalWrite(EC_dosingPump_low_B, HIGH); // low triggered relay
        mcp.digitalWrite(EC_dosingPump_high, HIGH);  // low triggered relay

        /////////rgb indicators high/low
        mcp.pinMode(MCP_TDS_RGB_LEDLOW, OUTPUT);
        mcp.pinMode(MCP_TDS_RGB_LEDOPTIMUM, OUTPUT);
        mcp.pinMode(MCP_TDS_RGB_LEDHIGH, OUTPUT);
        mcp.pinMode(MCP_PH_RGB_LEDLOW, OUTPUT);
        mcp.pinMode(MCP_PH_RGB_LEDOPTIMUM, OUTPUT);
        mcp.pinMode(MCP_PH_RGB_LEDHIGH, OUTPUT);
        mcp.digitalWrite(MCP_TDS_RGB_LEDLOW, LOW);
        mcp.digitalWrite(MCP_PH_RGB_LEDLOW, LOW);
        mcp.digitalWrite(MCP_TDS_RGB_LEDOPTIMUM, HIGH);
        mcp.digitalWrite(MCP_PH_RGB_LEDOPTIMUM, HIGH);
        mcp.digitalWrite(MCP_TDS_RGB_LEDHIGH, LOW);
        mcp.digitalWrite(MCP_PH_RGB_LEDHIGH, LOW);
    }

    void DosageSystem::tdsLow()
    {
        mcp.digitalWrite(MCP_TDS_RGB_LEDLOW, HIGH);
        mcp.digitalWrite(MCP_TDS_RGB_LEDOPTIMUM, LOW);
        mcp.digitalWrite(MCP_TDS_RGB_LEDHIGH, LOW);        
    }
    void DosageSystem::tdsHigh()
    {
        mcp.digitalWrite(MCP_TDS_RGB_LEDLOW, LOW);
        mcp.digitalWrite(MCP_TDS_RGB_LEDOPTIMUM, LOW);
        mcp.digitalWrite(MCP_TDS_RGB_LEDHIGH, HIGH);
    }
    void DosageSystem::tdsOptimum()
    {
        mcp.digitalWrite(MCP_TDS_RGB_LEDLOW, LOW);
        mcp.digitalWrite(MCP_TDS_RGB_LEDOPTIMUM, HIGH);
        mcp.digitalWrite(MCP_TDS_RGB_LEDHIGH, LOW);        
    }
    void DosageSystem::phLow()
    {
        mcp.digitalWrite(MCP_PH_RGB_LEDLOW, HIGH);
        mcp.digitalWrite(MCP_PH_RGB_LEDOPTIMUM, LOW);
        mcp.digitalWrite(MCP_PH_RGB_LEDHIGH, LOW);        
    }
    void DosageSystem::phHigh()
    {
        mcp.digitalWrite(MCP_PH_RGB_LEDLOW, LOW);
        mcp.digitalWrite(MCP_PH_RGB_LEDOPTIMUM, LOW);
        mcp.digitalWrite(MCP_PH_RGB_LEDHIGH, HIGH);
    }
    void DosageSystem::phOptimum()
    {
        mcp.digitalWrite(MCP_PH_RGB_LEDLOW, LOW);
        mcp.digitalWrite(MCP_PH_RGB_LEDOPTIMUM, HIGH);
        mcp.digitalWrite(MCP_PH_RGB_LEDHIGH, LOW);        
    }

    void DosageSystem::ValidateMeasuredReadingsPH(void)
    {
        if ((measuredParams.f_PHValue) > (setParams.f_PH_LowThresholdMin) &&
            (measuredParams.f_PHValue < setParams.f_PH_LowThresholdMax))
        {
            measuredParams.b_PH_isValid = true;
        }

        if (measuredParams.b_PH_isValid == true)
        {
            Serial.print("*************PH:");
            Serial.println(measuredParams.f_PHValue);

            if ((measuredParams.f_PHValue) <= setParams.f_PH_LowThreshold)
            {
                // Serial.print("PH:");
                // Serial.println(measure_data.ph);

                Serial.println("PH low flag");
                Serial.println(setParams.f_PH_LowThreshold);
                measuredParams.b_PH_LowFlag = true;
                phLow();
                //  Serial.println("low flag");
            }
            else if ((measuredParams.f_PHValue) >= setParams.f_PH_HighThreshold)
            {
                Serial.println("PH high flag");
                measuredParams.b_PH_HighFlag = true;
                phHigh();
            }
            else
            {
                phOptimum();
            }
        }
        // end of ph data analysis
    }
    void DosageSystem::ValidateMeasuredReadingsTDS(void)
    {        
        Serial.print("*************EC:");
        Serial.println(measuredParams.f_ECValue);

        if ((measuredParams.f_ECValue) > (setParams.f_EC_LowThresholdMin) && (measuredParams.f_ECValue < setParams.f_EC_HighThresholdMax)) //ec validation checking
        {
            measuredParams.b_EC_isValid = true;
        }

        if (measuredParams.b_EC_isValid == true)
        { // if ec validation is true

            if ((measuredParams.f_ECValue) <= setParams.f_EC_LowThreshold)
            {
                Serial.println("ec low flag");
                measuredParams.b_EC_LowFlag = true;
                tdsLow();
            }
            else if ((measuredParams.f_ECValue) >= setParams.f_EC_HighThreshold)
            {
                Serial.println("ec high flag");
                measuredParams.b_EC_HighFlag = true;
                tdsHigh();
            }
            else
            {
                tdsOptimum();
            }
        }
        // end of ec data analysis
    }
    void DosageSystem::ValidateMeasuredReadingsTEMP(void)
    {

        
        Serial.print("*************Temp:");
        Serial.println(measuredParams.f_TempValue);
        if ((measuredParams.f_TempValue) > (setParams.f_Temp_LowThresholdMin) && (measuredParams.f_TempValue < setParams.f_Temp_HighThresholdMax)) //temperature validation checking
        {
            measuredParams.b_Temp_isValid = true;
        }

        if (measuredParams.b_Temp_isValid == true)
        { //if temp validation is true

            if ((measuredParams.f_TempValue) <= (setParams.f_Temp_LowThreshold))
            {
                measuredParams.b_Temp_LowFlag = true;
            }
            if ((measuredParams.f_TempValue) >= setParams.f_Temp_HighThreshold)
            {
                measuredParams.b_Temp_HighFlag = true;
            }
        }
    }

    void DosageSystem::ClearTimers()
    {
        mcp.digitalWrite(PH_dosingPump_low, HIGH);   // low triggered relay
        mcp.digitalWrite(PH_dosingPump_high, HIGH);  // low triggered relay
        mcp.digitalWrite(EC_dosingPump_low_A, HIGH); // low triggered relay
        mcp.digitalWrite(EC_dosingPump_low_B, HIGH); // low triggered relay
        mcp.digitalWrite(EC_dosingPump_high, HIGH);  // low triggered relay
        measuredParams.u32_PHTimerCount = 0;
        measuredParams.u32_ECTimerCount = 0;
    }

    bool DosageSystem::Process()
    {
        bool retVal = true;
#if 1
        float diff_ph = 0.0f;
        float diff_ec = 0.0f;
        bool  dosageNotNeeded = false;

        //PH LOW DOSING
        if (measuredParams.b_PH_LowFlag == true)
        {
            Serial.println("************PH_Dosage_low Flag set******************");

            diff_ph = ((setParams.f_PH_LowThreshold) - (measuredParams.f_PHValue));
            {
                Serial.print("difference ph value _ ");
                Serial.println(diff_ph);

                if ((diff_ph >= 0.10) && (diff_ph <= 0.50))
                {
                    pH_Low_5_seconds();
                    //   pH_Low_10_seconds();
                }
                else if ((diff_ph > 0.50) && (diff_ph <= 1.00))
                {
                    pH_Low_5_seconds();
                    // pH_Low_15_seconds();
                }
                else if ((diff_ph > 1.00) && (diff_ph <= 1.50))
                {
                    pH_Low_5_seconds();
                    //  pH_Low_half_minute();
                }
                else if ((diff_ph > 1.50) && (diff_ph <= 2.00))
                {
                    pH_Low_5_seconds();
                   // pH_Low_10_seconds();
                    //   pH_Low_half_minute();
                    //   pH_Low_45_seconds();
                }
                else if ((diff_ph >= 2.10) && (diff_ph <= 2.50))
                {
                      pH_Low_5_seconds();
                    //pH_Low_10_seconds();
                    //   pH_Low_half_minute();
                    //   pH_Low_45_seconds();
                }
                else if ((diff_ph >= 2.60) && (diff_ph <= 3.50))
                {
                      pH_Low_5_seconds();
                   // pH_Low_10_seconds();
                    //  pH_Low_half_minute();
                    //   pH_Low_45_seconds();
                }
                else if ((diff_ph >= 3.60) && (diff_ph <= 4.50))
                {
                     pH_Low_5_seconds();
                   // pH_Low_15_seconds();
                   // pH_Low_half_minute();
                  //  pH_Low_45_seconds();
                }
                else
                {
                    dosageNotNeeded = true;
                    measuredParams.b_PH_LowFlag = false;
                }
            }
        }

            //PH HIGH DOSING
            if (measuredParams.b_PH_HighFlag == true)
            {

                Serial.println("************PH_Dosage_High Flag set******************");

                diff_ph = ((measuredParams.f_PHValue) - (setParams.f_PH_HighThreshold));
                {

                Serial.print("difference ph value - ");
                Serial.println(diff_ph);
                 
                if ((diff_ph >= 0.1) && (diff_ph <= 0.50))
                {
                    pH_High_5_seconds();
                    //pH_High_half_minute();
                }
                else if ((diff_ph >= 0.6) && (diff_ph <= 1.0))
                {
                    pH_High_5_seconds();
                    //pH_High_45_seconds();
                }
                else if ((diff_ph >= 1.1) && (diff_ph <= 1.50))
                {
                     pH_High_5_seconds();
                    //pH_High_10_seconds();
                    //pH_High_one_minute();
                }
                else if ((diff_ph >= 1.60) && (diff_ph <= 7.0))
                {
                      pH_High_5_seconds();
                   // pH_High_10_seconds();
                   // pH_High_two_minute();
                }
                else
                {
                    dosageNotNeeded = true;
                    measuredParams.b_PH_HighFlag = false;
                }
              }
            }

            // EC LOW DOSING

            if (measuredParams.b_EC_LowFlag == true)
            {
                Serial.println("************EC_Dosage_Low Flag set******************");
                diff_ec = ((setParams.f_EC_LowThreshold) - (measuredParams.f_ECValue));
             {
                Serial.print("difference ec value - ");
                Serial.println(diff_ec);

                if ((diff_ec >= 0) && (diff_ec <= 200))
                {
                    ec_Low_5_seconds();
                    //ec_Low_half_minute();
                }
                else if ((diff_ec > 200) && (diff_ec <= 400))
                {
                    ec_Low_5_seconds();
                    //ec_Low_45_seconds();
                }
                else if ((diff_ec > 400) && (diff_ec <= 600))
                {
                    ec_Low_5_seconds();
                   // ec_Low_one_minute();
                }
                else if ((diff_ec > 600) && (diff_ec <= 800))
                {
                    ec_Low_5_seconds();
                   // ec_Low_two_minute();
                }
                else
                {
                    dosageNotNeeded = true;
                    measuredParams.b_EC_LowFlag = false;
                }
              }
            }

            // EC high dosing

            if (measuredParams.b_EC_HighFlag == true)
            {
                Serial.println("************EC_Dosage_High Flag set******************");
                diff_ec = ((measuredParams.f_ECValue) - (setParams.f_EC_HighThreshold));
                {
                Serial.print("difference ec value - ");
                Serial.println(diff_ec);

                if ((diff_ec >= 0) && (diff_ec <= 100))
                {
                    ec_High_5_seconds();
                   // ec_High_half_minute();
                }
                else if ((diff_ec > 100) && (diff_ec <= 200))
                {
                    ec_High_5_seconds();
                   // ec_High_45_seconds();
                }
                else if ((diff_ec > 200) && (diff_ec <= 300))
                {
                    ec_High_5_seconds();
                    //ec_High_15_seconds();
                    //ec_High_one_minute();
                }
                else if ((diff_ec > 300) && (diff_ec <= 500))
                {
                    ec_High_5_seconds();
                   // ec_High_15_seconds();
                   // ec_High_two_minute();           
                }
                else
                {
                    dosageNotNeeded = true;
                    measuredParams.b_EC_HighFlag = false;
                }
              }
            }
#endif
                if( dosageNotNeeded == true )
                {
                    retVal = true;
                }

                if ((measuredParams.b_PH_LowFlag == true) || (measuredParams.b_PH_HighFlag == true) ||
                    (measuredParams.b_EC_LowFlag == true) || (measuredParams.b_EC_HighFlag == true))
                {
                    retVal = false;
                }                               
           /*
        if(mcp.readGPIO(PH_dosingPump_low)==LOW)
        {
            measuredParams.PH_dosingPump_lowTime++;
            Serial.print("PH Dosing Pump Low time= ");
            Serial.println(measuredParams.PH_dosingPump_lowTime);
        }
        if(mcp.readGPIO(PH_dosingPump_high)==LOW)
        {
            measuredParams.PH_dosingPump_highTime++;
            Serial.print("PH Dosing Pump High time= ");
            Serial.println(measuredParams.PH_dosingPump_highTime);
        }
        if(mcp.readGPIO(EC_dosingPump_low_A)==LOW)
        {
            measuredParams.EC_dosingPump_low_ATime++;
            Serial.print("EC Dosing Pump Low A time= ");
            Serial.println(measuredParams.EC_dosingPump_low_ATime);
        }
        if(mcp.readGPIO(EC_dosingPump_low_B)==LOW)
        {
            measuredParams.EC_dosingPump_low_BTime++;
            Serial.print("EC Dosing Pump Low B time= ");
            Serial.println(measuredParams.EC_dosingPump_low_BTime);
        }
        if(mcp.readGPIO(EC_dosingPump_high)==LOW)
        {
            measuredParams.EC_dosingPump_highTime++;
            Serial.print("EC Dosing Pump  time= ");
            Serial.println(measuredParams.EC_dosingPump_highTime);
        }
        */
        //Check these with thresholds for generating alerts         
        return retVal;
    }

    void DosageSystem::pH_Low_10_seconds()
    {
        Serial.print("pH_Low_10_seconds_20- ");
        Serial.println(measuredParams.u32_PHTimerCount);

        if ((measuredParams.u32_PHTimerCount == 10) || (measuredParams.u32_PHTimerCount == 11) || (measuredParams.u32_PHTimerCount == 12))
        {
            mcp.digitalWrite(PH_dosingPump_low, LOW);            
        }
        else if ((measuredParams.u32_PHTimerCount == 20) || (measuredParams.u32_PHTimerCount == 21) || (measuredParams.u32_PHTimerCount == 22))
        {
            mcp.digitalWrite(PH_dosingPump_low, HIGH);
            ////measuredParams.u32_PHTimerCount = 0;
            measuredParams.b_PH_LowFlag = 0;
        }
    }

    void DosageSystem::pH_Low_5_seconds()
    {
        Serial.print("pH_Low_5_seconds_15- "); //changed to 3 sec
        Serial.println(measuredParams.u32_PHTimerCount);

        if ((measuredParams.u32_PHTimerCount == 10) || (measuredParams.u32_PHTimerCount == 11) || (measuredParams.u32_PHTimerCount == 12))
        {
            mcp.digitalWrite(PH_dosingPump_low, LOW);
        }
        else if ((measuredParams.u32_PHTimerCount == 15) || (measuredParams.u32_PHTimerCount == 16) || (measuredParams.u32_PHTimerCount == 17))
        {
            mcp.digitalWrite(PH_dosingPump_low, HIGH);
            ////measuredParams.u32_PHTimerCount = 0;
            measuredParams.b_PH_LowFlag = 0;
        }
    }

    void DosageSystem::pH_Low_15_seconds()
    {
        Serial.print("pH_Low_15_seconds_25- ");
        Serial.println(measuredParams.u32_PHTimerCount);

        if ((measuredParams.u32_PHTimerCount == 10) || (measuredParams.u32_PHTimerCount == 11) || (measuredParams.u32_PHTimerCount == 12))
        {
            mcp.digitalWrite(PH_dosingPump_low, LOW);
        }
        else if ((measuredParams.u32_PHTimerCount == 25) || (measuredParams.u32_PHTimerCount == 26) || (measuredParams.u32_PHTimerCount == 27))
        {
            mcp.digitalWrite(PH_dosingPump_low, HIGH);
            //measuredParams.u32_PHTimerCount = 0;
            measuredParams.b_PH_LowFlag = 0;
        }
    }

    void DosageSystem::pH_Low_half_minute()
    {
        Serial.print("pH_Low_half_minute_40 - ");
        Serial.println(measuredParams.u32_PHTimerCount);

        if ((measuredParams.u32_PHTimerCount == 10) || (measuredParams.u32_PHTimerCount == 11) || (measuredParams.u32_PHTimerCount == 12))
        {
            mcp.digitalWrite(PH_dosingPump_low, LOW);
        }
        else if ((measuredParams.u32_PHTimerCount == 40) || (measuredParams.u32_PHTimerCount == 41) || (measuredParams.u32_PHTimerCount == 42))
        {
            mcp.digitalWrite(PH_dosingPump_low, HIGH);
            //measuredParams.u32_PHTimerCount = 0;
            measuredParams.b_PH_LowFlag = 0;
        }
    }

    void DosageSystem::pH_Low_45_seconds()
    {
        Serial.print("pH_Low_45_seconds_55- ");
        Serial.println(measuredParams.u32_PHTimerCount);

        if ((measuredParams.u32_PHTimerCount == 10) || (measuredParams.u32_PHTimerCount == 11) || (measuredParams.u32_PHTimerCount == 12))
        {
            mcp.digitalWrite(PH_dosingPump_low, LOW);
        }
        else if ((measuredParams.u32_PHTimerCount == 55) || (measuredParams.u32_PHTimerCount == 56) || (measuredParams.u32_PHTimerCount == 57))
        {
            mcp.digitalWrite(PH_dosingPump_low, HIGH);
            //measuredParams.u32_PHTimerCount = 0;
            measuredParams.b_PH_LowFlag = 0;
        }
    }

    void DosageSystem::pH_Low_one_minute()
    {
        Serial.print("pH_Low_one_minute_70- ");
        Serial.println(measuredParams.u32_PHTimerCount);

        if ((measuredParams.u32_PHTimerCount == 10) || (measuredParams.u32_PHTimerCount == 11) || (measuredParams.u32_PHTimerCount == 12))
        {
            mcp.digitalWrite(PH_dosingPump_low, LOW);
        }
        else if ((measuredParams.u32_PHTimerCount == 70) || (measuredParams.u32_PHTimerCount == 71) || (measuredParams.u32_PHTimerCount == 72))
        {
            mcp.digitalWrite(PH_dosingPump_low, HIGH);
            //measuredParams.u32_PHTimerCount = 0;
            measuredParams.b_PH_LowFlag = 0;
        }
    }
    void DosageSystem::pH_Low_two_minute()
    {
        Serial.print("pH_Low_two_minute_130- ");
        Serial.println(measuredParams.u32_PHTimerCount);

        if ((measuredParams.u32_PHTimerCount == 10) || (measuredParams.u32_PHTimerCount == 11) || (measuredParams.u32_PHTimerCount == 12))
        {
            mcp.digitalWrite(PH_dosingPump_low, LOW);
        }
        else if ((measuredParams.u32_PHTimerCount == 130) || (measuredParams.u32_PHTimerCount == 131) || (measuredParams.u32_PHTimerCount == 132))
        {
            mcp.digitalWrite(PH_dosingPump_low, HIGH);
            //measuredParams.u32_PHTimerCount = 0;
            measuredParams.b_PH_LowFlag = 0;
        }
    }

    void DosageSystem::pH_High_5_seconds()
    {
        Serial.print("pH_High_5_seconds_15- ");
        //changed to 3 sec
        Serial.println(measuredParams.u32_PHTimerCount);

        if ((measuredParams.u32_PHTimerCount == 10) || (measuredParams.u32_PHTimerCount == 11) || (measuredParams.u32_PHTimerCount == 12))
        {
            mcp.digitalWrite(PH_dosingPump_high, LOW);
        }
        else if ((measuredParams.u32_PHTimerCount == 15) || (measuredParams.u32_PHTimerCount == 16) || (measuredParams.u32_PHTimerCount == 17))
        {
            mcp.digitalWrite(PH_dosingPump_high, HIGH);
            //measuredParams.u32_PHTimerCount = 0;
            measuredParams.b_PH_HighFlag = 0;
        }
    }

    void DosageSystem::pH_High_10_seconds()
    {
        Serial.print("pH_High_10_seconds_20- ");
        Serial.println(measuredParams.u32_PHTimerCount);

        if ((measuredParams.u32_PHTimerCount == 10) || (measuredParams.u32_PHTimerCount == 11) || (measuredParams.u32_PHTimerCount == 12))
        {
            mcp.digitalWrite(PH_dosingPump_high, LOW);
        }
        else if ((measuredParams.u32_PHTimerCount == 20) || (measuredParams.u32_PHTimerCount == 21) || (measuredParams.u32_PHTimerCount == 22))
        {
            mcp.digitalWrite(PH_dosingPump_high, HIGH);
            //measuredParams.u32_PHTimerCount = 0;
            measuredParams.b_PH_HighFlag = 0;
        }
    }

    void DosageSystem::pH_High_15_seconds()
    {
        Serial.print("pH_High_15_seconds_25- ");
        Serial.println(measuredParams.u32_PHTimerCount);

        if ((measuredParams.u32_PHTimerCount == 10) || (measuredParams.u32_PHTimerCount == 11) || (measuredParams.u32_PHTimerCount == 12))
        {
            mcp.digitalWrite(PH_dosingPump_high, LOW);
        }
        else if ((measuredParams.u32_PHTimerCount == 25) || (measuredParams.u32_PHTimerCount == 26) || (measuredParams.u32_PHTimerCount == 27))
        {
            mcp.digitalWrite(PH_dosingPump_high, HIGH);
            //measuredParams.u32_PHTimerCount = 0;
            measuredParams.b_PH_HighFlag = 0;
        }
    }

    void DosageSystem::pH_High_half_minute()
    {
        Serial.print("pH_High_half_minute_40- ");
        Serial.println(measuredParams.u32_PHTimerCount);

        if ((measuredParams.u32_PHTimerCount == 10) || (measuredParams.u32_PHTimerCount == 11) || (measuredParams.u32_PHTimerCount == 12))
        {
            mcp.digitalWrite(PH_dosingPump_high, LOW);
        }
        else if ((measuredParams.u32_PHTimerCount == 40) || (measuredParams.u32_PHTimerCount == 41) || (measuredParams.u32_PHTimerCount == 42))
        {
            mcp.digitalWrite(PH_dosingPump_high, HIGH);
            //measuredParams.u32_PHTimerCount = 0;
            measuredParams.b_PH_HighFlag = 0;
        }
    }

    void DosageSystem::pH_High_45_seconds()
    {
        Serial.print("pH_High_45_seconds_55- ");
        Serial.println(measuredParams.u32_PHTimerCount);

        if ((measuredParams.u32_PHTimerCount == 10) || (measuredParams.u32_PHTimerCount == 11) || (measuredParams.u32_PHTimerCount == 12))
        {
            mcp.digitalWrite(PH_dosingPump_high, LOW);
        }
        else if ((measuredParams.u32_PHTimerCount == 55) || (measuredParams.u32_PHTimerCount == 56) || (measuredParams.u32_PHTimerCount == 57))
        {
            mcp.digitalWrite(PH_dosingPump_high, HIGH);
            // measuredParams.u32_PHTimerCount = 0;
            measuredParams.b_PH_HighFlag = 0;
        }
    }

    void DosageSystem::pH_High_one_minute()
    {
        Serial.print("pH_High_one_minute_70- ");
        Serial.println(measuredParams.u32_PHTimerCount);

        if ((measuredParams.u32_PHTimerCount == 10) || (measuredParams.u32_PHTimerCount == 11) || (measuredParams.u32_PHTimerCount == 12))
        {
            mcp.digitalWrite(PH_dosingPump_high, LOW);
        }
        else if ((measuredParams.u32_PHTimerCount == 70) || (measuredParams.u32_PHTimerCount == 71) || (measuredParams.u32_PHTimerCount == 72))
        {
            mcp.digitalWrite(PH_dosingPump_high, HIGH);
            //measuredParams.u32_PHTimerCount = 0;
            measuredParams.b_PH_HighFlag = 0;
        }
    }

    void DosageSystem::pH_High_two_minute()
    {
        Serial.print("pH_High_two_minute_130- ");
        Serial.println(measuredParams.u32_PHTimerCount);

        if ((measuredParams.u32_PHTimerCount == 10) || (measuredParams.u32_PHTimerCount == 11) || (measuredParams.u32_PHTimerCount == 12))
        {
            mcp.digitalWrite(PH_dosingPump_high, LOW);
        }
        else if ((measuredParams.u32_PHTimerCount == 130) || (measuredParams.u32_PHTimerCount == 131) || (measuredParams.u32_PHTimerCount == 132))
        {
            mcp.digitalWrite(PH_dosingPump_high, HIGH);
            //measuredParams.u32_PHTimerCount = 0;
            measuredParams.b_PH_HighFlag = 0;
        }
    }

    void DosageSystem::ec_Low_one_minute()
    {
        Serial.print("ec_Low_one_minute_70- ");
        Serial.println(measuredParams.u32_ECTimerCount);

        if ((measuredParams.u32_ECTimerCount == 10) || (measuredParams.u32_ECTimerCount == 11) || (measuredParams.u32_ECTimerCount == 12))
        {
            mcp.digitalWrite(EC_dosingPump_low_A, LOW);
            mcp.digitalWrite(EC_dosingPump_low_B, LOW);
        }
        else if ((measuredParams.u32_ECTimerCount == 70) || (measuredParams.u32_ECTimerCount == 71) || (measuredParams.u32_ECTimerCount == 72))
        {
            mcp.digitalWrite(EC_dosingPump_low_A, HIGH);
            mcp.digitalWrite(EC_dosingPump_low_B, HIGH);
            //measuredParams.u32_ECTimerCount = 0;
            measuredParams.b_EC_LowFlag = 0;
        }
    }
    void DosageSystem::ec_Low_two_minute()
    {
        Serial.print("ec_Low_two_minute_130- ");
        Serial.println(measuredParams.u32_ECTimerCount);

        if ((measuredParams.u32_ECTimerCount == 10) || (measuredParams.u32_ECTimerCount == 11) || (measuredParams.u32_ECTimerCount == 12))
        {
            mcp.digitalWrite(EC_dosingPump_low_A, LOW);
            mcp.digitalWrite(EC_dosingPump_low_B, LOW);
        }
        else if ((measuredParams.u32_ECTimerCount == 130) || (measuredParams.u32_ECTimerCount == 131) || (measuredParams.u32_ECTimerCount == 132))
        {
            mcp.digitalWrite(EC_dosingPump_low_A, HIGH);
            mcp.digitalWrite(EC_dosingPump_low_B, HIGH);
            //measuredParams.u32_ECTimerCount = 0;
            measuredParams.b_EC_LowFlag = 0;
        }
    }

    void DosageSystem::ec_Low_5_seconds()
    {
        Serial.print("ec_low_5_seconds_15- ");
        Serial.println(measuredParams.u32_ECTimerCount);

        if ((measuredParams.u32_ECTimerCount == 10) || (measuredParams.u32_ECTimerCount == 11) || (measuredParams.u32_ECTimerCount == 12))
        {
            mcp.digitalWrite(EC_dosingPump_low_A, LOW);
            mcp.digitalWrite(EC_dosingPump_low_B, LOW);
        }
        else if ((measuredParams.u32_ECTimerCount == 15) || (measuredParams.u32_ECTimerCount == 16) || (measuredParams.u32_ECTimerCount == 17))
        {
            mcp.digitalWrite(EC_dosingPump_low_A, HIGH);
            mcp.digitalWrite(EC_dosingPump_low_B, HIGH);
            //measuredParams.u32_ECTimerCount = 0;
            measuredParams.b_EC_LowFlag = 0;
        }
    }

    void DosageSystem::ec_Low_10_seconds()
    {
        Serial.print("ec_low_10_seconds_20- ");
        Serial.println(measuredParams.u32_ECTimerCount);

        if ((measuredParams.u32_ECTimerCount == 10) || (measuredParams.u32_ECTimerCount == 11) || (measuredParams.u32_ECTimerCount == 12))
        {
            mcp.digitalWrite(EC_dosingPump_low_A, LOW);
            mcp.digitalWrite(EC_dosingPump_low_B, LOW);
        }
        else if ((measuredParams.u32_ECTimerCount == 20) || (measuredParams.u32_ECTimerCount == 21) || (measuredParams.u32_ECTimerCount == 22))
        {
            mcp.digitalWrite(EC_dosingPump_low_A, HIGH);
            mcp.digitalWrite(EC_dosingPump_low_B, HIGH);
            //measuredParams.u32_ECTimerCount = 0;
            measuredParams.b_EC_LowFlag = 0;
        }
    }

    void DosageSystem::ec_Low_15_seconds()
    {
        Serial.print("ec_low_15_seconds_25- ");
        Serial.println(measuredParams.u32_ECTimerCount);

        if ((measuredParams.u32_ECTimerCount == 10) || (measuredParams.u32_ECTimerCount == 11) || (measuredParams.u32_ECTimerCount == 12))
        {
            mcp.digitalWrite(EC_dosingPump_low_A, LOW);
            mcp.digitalWrite(EC_dosingPump_low_B, LOW);
        }
        else if ((measuredParams.u32_ECTimerCount == 25) || (measuredParams.u32_ECTimerCount == 26) || (measuredParams.u32_ECTimerCount == 27))
        {
            mcp.digitalWrite(EC_dosingPump_low_A, HIGH);
            mcp.digitalWrite(EC_dosingPump_low_B, HIGH);
            //measuredParams.u32_ECTimerCount = 0;
            measuredParams.b_EC_LowFlag = 0;
        }
    }

    void DosageSystem::ec_Low_45_seconds()
    {
        Serial.print("ec_Low_45_seconds_55- ");
        Serial.println(measuredParams.u32_ECTimerCount);

        if ((measuredParams.u32_ECTimerCount == 10) || (measuredParams.u32_ECTimerCount == 11) || (measuredParams.u32_ECTimerCount == 12))
        {
            mcp.digitalWrite(EC_dosingPump_low_A, LOW);
            mcp.digitalWrite(EC_dosingPump_low_B, LOW);
        }
        else if ((measuredParams.u32_ECTimerCount == 55) || (measuredParams.u32_ECTimerCount == 56) || (measuredParams.u32_ECTimerCount == 57))
        {
            mcp.digitalWrite(EC_dosingPump_low_A, HIGH);
            mcp.digitalWrite(EC_dosingPump_low_B, HIGH);
            //measuredParams.u32_ECTimerCount = 0;
            measuredParams.b_EC_LowFlag = 0;
        }
    }
    void DosageSystem::ec_Low_half_minute()
    {
        Serial.print("ec_Low_half_minute_40- ");
        Serial.println(measuredParams.u32_ECTimerCount);

        if ((measuredParams.u32_ECTimerCount == 10) || (measuredParams.u32_ECTimerCount == 11) || (measuredParams.u32_ECTimerCount == 12))
        {
            mcp.digitalWrite(EC_dosingPump_low_A, LOW);
            mcp.digitalWrite(EC_dosingPump_low_B, LOW);
        }
        else if ((measuredParams.u32_ECTimerCount == 40) || (measuredParams.u32_ECTimerCount == 41) || (measuredParams.u32_ECTimerCount == 42))
        {
            mcp.digitalWrite(EC_dosingPump_low_A, HIGH);
            mcp.digitalWrite(EC_dosingPump_low_B, HIGH);
            //measuredParams.u32_ECTimerCount = 0;
            measuredParams.b_EC_LowFlag = 0;
        }
    }
    void DosageSystem::ec_High_one_minute()
    {
        Serial.print("ec_High_one_minute_70- ");
        Serial.println(measuredParams.u32_ECTimerCount);

        if ((measuredParams.u32_ECTimerCount == 10) || (measuredParams.u32_ECTimerCount == 11) || (measuredParams.u32_ECTimerCount == 12))
        {
            mcp.digitalWrite(EC_dosingPump_high, LOW);
        }
        else if ((measuredParams.u32_ECTimerCount == 70) || (measuredParams.u32_ECTimerCount == 71) || (measuredParams.u32_ECTimerCount == 72))
        {
            mcp.digitalWrite(EC_dosingPump_high, HIGH);
            //measuredParams.u32_ECTimerCount = 0;
            measuredParams.b_EC_HighFlag = 0;
        }
    }
    void DosageSystem::ec_High_two_minute()
    {
        Serial.print("ec_High_two_minute_130- ");
        Serial.println(measuredParams.u32_ECTimerCount);

        if ((measuredParams.u32_ECTimerCount == 10) || (measuredParams.u32_ECTimerCount == 11) || (measuredParams.u32_ECTimerCount == 12))
        {
            mcp.digitalWrite(EC_dosingPump_high, LOW);
        }
        else if ((measuredParams.u32_ECTimerCount == 130) || (measuredParams.u32_ECTimerCount == 131) || (measuredParams.u32_ECTimerCount == 132))
        {
            mcp.digitalWrite(EC_dosingPump_high, HIGH);
            //measuredParams.u32_ECTimerCount = 0;
            measuredParams.b_EC_HighFlag = 0;
        }
    }

    void DosageSystem::ec_High_15_seconds()
    {
        Serial.print("ec_High_15_seconds_25- ");
        Serial.println(measuredParams.u32_ECTimerCount);

        if ((measuredParams.u32_ECTimerCount == 10) || (measuredParams.u32_ECTimerCount == 11) || (measuredParams.u32_ECTimerCount == 12))
        {
            mcp.digitalWrite(EC_dosingPump_high, LOW);
        }
        else if ((measuredParams.u32_ECTimerCount == 25) || (measuredParams.u32_ECTimerCount == 26) || (measuredParams.u32_ECTimerCount == 27))
        {
            mcp.digitalWrite(EC_dosingPump_high, HIGH);
            //measuredParams.u32_ECTimerCount = 0;
            measuredParams.b_EC_HighFlag = 0;
        }
    }

    void DosageSystem::ec_High_10_seconds()
    {
        Serial.print("ec_High_10_seconds_20- ");
        Serial.println(measuredParams.u32_ECTimerCount);

        if ((measuredParams.u32_ECTimerCount == 10) || (measuredParams.u32_ECTimerCount == 11) || (measuredParams.u32_ECTimerCount == 12))
        {
            mcp.digitalWrite(EC_dosingPump_high, LOW);
        }
        else if ((measuredParams.u32_ECTimerCount == 20) || (measuredParams.u32_ECTimerCount == 21) || (measuredParams.u32_ECTimerCount == 22))
        {
            mcp.digitalWrite(EC_dosingPump_high, HIGH);
            //measuredParams.u32_ECTimerCount = 0;
            measuredParams.b_EC_HighFlag = 0;
        }
    }

    void DosageSystem::ec_High_5_seconds()
    {
        Serial.print("ec_High_5_seconds_15- ");
        Serial.println(measuredParams.u32_ECTimerCount);

        if ((measuredParams.u32_ECTimerCount == 10) || (measuredParams.u32_ECTimerCount == 11) || (measuredParams.u32_ECTimerCount == 12))
        {
            mcp.digitalWrite(EC_dosingPump_high, LOW);
        }
        else if ((measuredParams.u32_ECTimerCount == 15) || (measuredParams.u32_ECTimerCount == 16) || (measuredParams.u32_ECTimerCount == 17))
        {
            mcp.digitalWrite(EC_dosingPump_high, HIGH);
            //measuredParams.u32_ECTimerCount = 0;
            measuredParams.b_EC_HighFlag = 0;
        }
    }

    void DosageSystem::ec_High_45_seconds()
    {
        Serial.print("ec_High_45_seconds_55- ");
        Serial.println(measuredParams.u32_ECTimerCount);

        if ((measuredParams.u32_ECTimerCount == 10) || (measuredParams.u32_ECTimerCount == 11) || (measuredParams.u32_ECTimerCount == 12))
        {
            mcp.digitalWrite(EC_dosingPump_high, LOW);
        }
        else if ((measuredParams.u32_ECTimerCount == 55) || (measuredParams.u32_ECTimerCount == 56) || (measuredParams.u32_ECTimerCount == 57))
        {
            mcp.digitalWrite(EC_dosingPump_high, HIGH);
            //measuredParams.u32_ECTimerCount = 0;
            measuredParams.b_EC_HighFlag = 0;
        }
    }
    void DosageSystem::ec_High_half_minute()
    {
        Serial.print("ec_High_half_minute_40- ");
        Serial.println(measuredParams.u32_ECTimerCount);

        if ((measuredParams.u32_ECTimerCount == 10) || (measuredParams.u32_ECTimerCount == 11) || (measuredParams.u32_ECTimerCount == 12))
        {
            mcp.digitalWrite(EC_dosingPump_high, LOW);
        }
        else if ((measuredParams.u32_ECTimerCount == 40) || (measuredParams.u32_ECTimerCount == 41) || (measuredParams.u32_ECTimerCount == 42))
        {
            mcp.digitalWrite(EC_dosingPump_high, HIGH);
            //measuredParams.u32_ECTimerCount = 0;
            measuredParams.b_EC_HighFlag = 0;
        }
    }
}
