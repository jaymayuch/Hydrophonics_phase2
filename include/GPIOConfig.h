#pragma once
#define TEMPSENSORPIN         25
#define PH_PIN                36
#define EC_PIN                39
#define MCP_SDA_PIN           21
#define MCP_SCL_PIN           22
      
#define PH_SUPPLY_VCC         13
#define PH_SUPPLY_GND         12
      
#define TS_SUPPLY_VCC         26
#define TS_SUPPLY_GND         27
      
      
#define MODEM_RX_PIN          2 
#define MODEM_TX_PIN          15
      
#define WATER_PUMP            16
#define AC2                   17
#define MODEM_POWER_SWITCH    19
#define AC4                   4 
      
#define ULTRA_TRIG_PIN        5 
#define ULTRA_ECHO_PIN        18

//MCP Pin Config

#define MCP_DOSING_PUMP_1     8
#define MCP_DOSING_PUMP_2     9
#define MCP_DOSING_PUMP_3     10
#define MCP_DOSING_PUMP_4     11
#define MCP_DOSING_PUMP_5     12
#define MCP_DOSING_PUMP_6     13
#define MCP_SOLENIOD_VALVE_1  14
#define MCP_SOLENIOD_VALVE_2  15
#define MCP_SOLENIOD_VALVE_3  0
#define MCP_SOLENIOD_VALVE_4  1

#define MCP_TDS_RGB_LEDHIGH    2
#define MCP_TDS_RGB_LEDOPTIMUM 3 
#define MCP_TDS_RGB_LEDLOW     4

#define MCP_PH_RGB_LEDHIGH     5
#define MCP_PH_RGB_LEDOPTIMUM  6
#define MCP_PH_RGB_LEDLOW      7

#define TIMER_INSTANCE0        0

#define DEMO_MODE              1

//EEPROM Storage
#define SIGNATURE              (0xDEADBEEF)
#define CONFIG_VERSION         0x00000003


#define SIGNATURE_ADDRESS      (0x00000000)
#define CONFIG_VERSION_ADDRESS  (SIGNATURE_ADDRESS+sizeof(unsigned int))
#define PH_CALIBRATION_ADDRESS  (CONFIG_VERSION_ADDRESS+sizeof(unsigned int))           //Future use 16 bytes

#define EC_CALIBRATION_ADDRESS  (PH_CALIBRATION_ADDRESS+(sizeof(float)*2))+16  //Future use 16 bytes

#define SET_PARAMS_ADDRESS      (EC_CALIBRATION_ADDRESS+(sizeof(float)*2))+16  

//#define CALIBRATION


