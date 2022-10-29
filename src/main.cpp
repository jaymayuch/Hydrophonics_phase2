#include <Arduino.h>
#include "Sensors.h"
#include "DosageSystem.h"
#include "LTEModem.h"
#include "MeasureSetParameters.h"
#include "GPIOConfig.h"
#include "TaskScheduler.h"


Scheduler runner;

void modemTaskProcess();
void hydrophonicSensProcess();

Task modemTask(2000,TASK_FOREVER, &modemTaskProcess,&runner,true);
Task hydrophonicSensTask(1000,TASK_FOREVER, &hydrophonicSensProcess,&runner,true);


//#define TFT_DISPLAY
//#define BLE_SERIAL

#ifdef BLE_SERIAL

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
#endif

#ifdef TFT_DISPLAY
#include "Display.h"
using namespace DisplayInterface;
#endif

using namespace SensorInterface;
using namespace DosageSystemInterface;
using namespace ModemInterface;

#define ONE_SEC_PERIOD_MEASUREMENT 1000
#define ONE_10_MSEC_PERIOD         500



DosageSystem dosageSystem(measParams, setParams);
SensorInterface::Sensors hydroSensors(SensorInterface::eSensorTypes::eNUM_SENSORS, setParams, measParams, dosageSystem);
Modem rfModem(measParams, setParams);

void modemTaskProcess()
{
#ifndef CALIBRATION
  Serial.println("Modem Process:");  
  rfModem.sendIOTServer();
#endif  
}
void hydrophonicSensProcess()
{
  //Serial.println("Dosage Process:");  
  dosageSystem.onTimer();
  hydroSensors.MeasurementProcess();
}


#ifdef TFT_DISPLAY
Display display(measParams);
#endif

hw_timer_t *timer0 = NULL;
volatile uint32_t u32_SysTick = 0;

void SystemTimer()
{
  u32_SysTick++;
}

void initAnlgDigitalPorts()
{
  // AC SUPPLY PORTS
  pinMode(WATER_PUMP, OUTPUT);
  pinMode(AC2, OUTPUT);
  pinMode(MODEM_POWER_SWITCH, OUTPUT);
  pinMode(AC4, OUTPUT);
  digitalWrite(WATER_PUMP, HIGH);
  digitalWrite(AC2, HIGH);
  digitalWrite(MODEM_POWER_SWITCH, HIGH);
  digitalWrite(AC4, HIGH);
  ///TRANSISTOR SWITCHES
  pinMode(TS_SUPPLY_GND, OUTPUT);
  pinMode(TS_SUPPLY_VCC, OUTPUT);
  digitalWrite(TS_SUPPLY_VCC, HIGH);
  pinMode(PH_SUPPLY_GND, OUTPUT);
  pinMode(PH_SUPPLY_VCC, OUTPUT);
  digitalWrite(PH_SUPPLY_VCC, HIGH);
}

void setup()
{
  Serial.begin(115200);

#ifdef BLE_SERIAL
  SerialBT.begin("ESP32_HYDRO"); //Bluetooth device name
#endif

  //EEPROM.begin(SET_PARAMS_ADDRESS+sizeof(setParams)); //needed EEPROM.begin to store calibration k in eeprom
  EEPROM.begin(32);
  LoadSetParameters(&setParams);
  initAnlgDigitalPorts();
  /*
    for(uint8_t byte=0;byte<8;byte++)
  {
    EEPROM.write(0x00+byte,0xFF);

  }
  */
  hydroSensors.Init();
  dosageSystem.Setup();



  timer0 = timerBegin(TIMER_INSTANCE0, 80, true);
  timerAttachInterrupt(timer0, &SystemTimer, true);
  timerAlarmWrite(timer0, 1000, true);
  timerAlarmEnable(timer0);
  rfModem.init();
#ifdef TFT_DISPLAY
  display.init();
#endif
}
void loop()
{
  runner.execute();
  if ((u32_SysTick % ONE_SEC_PERIOD_MEASUREMENT) == 0)
  {
    //dosageSystem.onTimer();
    //hydroSensors.MeasurementProcess();
#ifdef TFT_DISPLAY
    display.process();
#endif
static uint32_t count = 0;
#ifndef CALIBRATION
  if(count++%1==0)
  {
     rfModem.sendIOTServer();
    //rfModem.sendSMSAlert("9908072975");
  }  
#endif     
  }
}