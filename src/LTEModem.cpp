#include "LTEModem.h"
//#include "Adafruit_FONA.h"
//#include "defines.h"
//#include "TinyGsmClient.h"
//#define WIFI

#ifdef WIFI
#define TINY_GSM_USE_GPRS false
#define TINY_GSM_USE_WIFI true
#else
#define TINY_GSM_USE_GPRS false
#define TINY_GSM_USE_WIFI true
#endif

#include "GPIOConfig.h"
#include "ArduinoJson.h"
#define DUMP_AT_COMMANDS
#ifdef WIFI
#define TINY_GSM_MODEM_ESP8266
#else
#define TINY_GSM_MODEM_SIM7600
#endif
#include <TinyGsmClient.h>

#define apn         "airtelgprs.com"
#define gprsUser    ""    //"wapuser1"
#define gprsPass    ""    //"wap"
#define SerialMon Serial
#define SerialAT  Serial1
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23

#define MODEM_TX             MODEM_TX_PIN
#define MODEM_RX             MODEM_RX_PIN

#define DEVICE_ID           "111111111111111"


#ifdef WIFI
const char wifiSSID[] = "MrMrsChippada";
const char wifiPass[] = "mayukh13m";
#endif


namespace ModemInterface
{
#define MODEM_DEBUG    
#ifdef MODEM_DEBUG
    #define SerialMon Serial
    #define TINY_GSM_DEBUG SerialMon
    #include <StreamDebugger.h>
    StreamDebugger debugger(SerialAT, SerialMon);
    TinyGsm        modem(debugger);
#else
    TinyGsm modem(SerialAT);
#endif    
    TinyGsmClient client(modem);
    //TinyGsmClient client(modem);
    //BlynkTimer timer; // Create a Timer object called "timer"!
    bool GSM_CONNECT_OK = false;
    bool GSM_reconnect = false;

    //PH,EC,temp,motor status,on time,motor off time
    //Lights status,on time,off time
    // void sendUptime()
    // {
    //     Serial.println("Send sensor values");
    //     Serial.print(measParams.f_ECValue);        
    //     Serial.print(measParams.f_TempValue);
    //     Serial.print(measParams.f_PHValue);
    //     Serial.println();

    //     Blynk_GSM.virtualWrite(V0, measParams.f_ECValue);
    //     Blynk_GSM.virtualWrite(V1, measParams.f_TempValue);
    //     Blynk_GSM.virtualWrite(V2, measParams.f_PHValue);
    //     Blynk_GSM.virtualWrite(V3, measParams.waterPumpOnStatus);
    //     Blynk_GSM.virtualWrite(V4, measParams.waterPumpOnTime);
    //     Blynk_GSM.virtualWrite(V5, measParams.waterPumpOffTime);
    //     Blynk_GSM.virtualWrite(V6, measParams.lightPumpOnStatus);
    //     Blynk_GSM.virtualWrite(V7, measParams.lightPumpOnTime);
    //     Blynk_GSM.virtualWrite(V8, measParams.lightPumpOffTime);
    //     Blynk_GSM.virtualWrite(V9, measParams.waterLevelInCm);
    //     //Alert yet to be defined
    // }

    void Modem::init()
    {
        SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX,false);
        delay(3000);
        //timer.setInterval(4000L, sendUptime);
        //modem.init(); //modem.restart(); 
        //modem.setBaud(9600);
        //SerialAT.begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX);
        //Blynk_GSM.config(modem, gsm_blynk_tok, blynk_server, BLYNK_HARDWARE_PORT);
        //GSM_CONNECT_OK = Blynk_GSM.connectNetwork(apn, gprsUser, gprsPass);
    }
    
    //TODO to come from provisioning
    char server[] = "192.236.161.98";
    uint16_t port = 9000;
    char dataToSend[4096] = {0};

    void Modem::sendIOTServer(void)
    {            
        static uint32_t seconds_elapsed;
                 
        DynamicJsonDocument JsonCmd(4096);
        DynamicJsonDocument Response(4096);
        uint8_t valid =0;
        if (!GSM_CONNECT_OK)        
        {
            #ifdef WIFI
              if (!modem.networkConnect(wifiSSID, wifiPass)) {
                SerialMon.println(" fail");
                delay(10000);
                 return;
                }
                 GSM_CONNECT_OK = 1;
            #endif
//#ifdef MODEM_POWER_CONTROL            
            //Turn on  Modem logic and wait for 5sec
            
            do
            {
                yield();
                delayMicroseconds(5000000); //5secs
                digitalWrite(MODEM_POWER_SWITCH, LOW);
            }while(!modem.testAT(100));
//#endif
            modem.waitForNetwork(600000L);
             Serial.println("Modem connecting");
            #ifndef WIFI   
            modem.gprsConnect(apn, gprsUser, gprsPass);
            GSM_CONNECT_OK = modem.isGprsConnected();
            if(GSM_CONNECT_OK)
            Serial.println("Modem conneced");
            #endif
            client.connect(server, port,10);                        
        }
        else
        {            
            seconds_elapsed++;
/*
            if(seconds_elapsed%10==0)
            {
               Serial.println("GSM connected received"); 
               JsonCmd["cmd"] = "HEART_BEAT";
               JsonCmd["mac_address"] = "211111111111112";     
               serializeJson(JsonCmd, dataToSend);
               client.write(dataToSend);
                Serial.println("Heart Beat Sent");  
            }
*/
                               
            String CommandRecvd,EntryCommand;
            
             if(client.available())
             {                
                //uint8_t maxchar_before_brack = 10;
                 //delayMicroseconds(1000000);
                 
                //  while((CommandRecvd.indexOf('}') == -1)&&(maxchar_before_brack))
                //  {
                //     maxchar_before_brack--;
                //      ParsedCommand = client.readString();
                //      Serial.print(ParsedCommand);
                //      CommandRecvd.concat(ParsedCommand); 
                //  }
                 CommandRecvd.clear();
                 EntryCommand.clear();     
                 CommandRecvd = client.readString();          
                 Serial.print(CommandRecvd);                    
                 uint8_t index = 0;      
                if(!CommandRecvd.isEmpty())
                {
                do
                {
                    if(!EntryCommand.isEmpty())
                    {
                        if(CommandRecvd.length() > EntryCommand.indexOf('}')+1)
                        {
                            
                            EntryCommand = CommandRecvd.substring( index+1,CommandRecvd.indexOf('}',index+1)+1);
                            EntryCommand = EntryCommand.substring(EntryCommand.indexOf('{'),EntryCommand.indexOf('}')+1);
                            index = CommandRecvd.indexOf('}',index+1);                            
                        }
                    }
                    else
                    {             
                        EntryCommand.clear();
                         EntryCommand = CommandRecvd.substring(CommandRecvd.indexOf('{'),CommandRecvd.indexOf('}')+1);
                         index = CommandRecvd.indexOf('}');
                    }

                            
                //  while (!CommandRecvd.startsWith("{")&&(!CommandRecvd.isEmpty()))
                //  {
                //     CommandRecvd = CommandRecvd.substring(index,CommandRecvd.indexOf('}')+1); 
                //     index++; 
                //     Serial.print("\r\n"); 
                //     Serial.print(CommandRecvd);                                       
                //  }
               
                               
                //  Serial.println("parsed");
                //  Serial.println("Modem command Received");
                //  Serial.print(CommandRecvd);
                //  Serial.println("Entr command");
                //  Serial.print(EntryCommand);
                //  Serial.print(CommandRecvd.length() );
                //  Serial.print(EntryCommand.indexOf('}')+1);    
                //  Serial.print(index);    


            if(!EntryCommand.isEmpty())            
            {                
                Serial.println("********Valid Command Received********");  
                                
                deserializeJson(JsonCmd, EntryCommand);                

                 if(strcmp(JsonCmd["cmd"], "CONNECT_RESPONSE") == 0)                      
                 {                   
                       Response["cmd"] = "INIT";
                       Response["mac_address"] = DEVICE_ID;//modem.getIMEI();
                       Response["uid"] = JsonCmd["uid"];  
                       if(GSM_reconnect)
                            Response["network_resume"] = 1;       
                        else
                            Response["network_resume"] = 0;                                  
                       Serial.println("*****************CONNECT_RESPONSE Recvd**********");  
                     valid =1;
                 }                 

                 if(strcmp(JsonCmd["cmd"], "INIT_RESPONSE") == 0)                      
                 {            
                     Serial.println("*****************INIT_RESPONSE Recvd**********");       
                     Serial.println("Init Response Received");  
                     valid =0;
                     setParams.u32_MeasureFreqInSec = 5;
                     //Response["cmd"] = NULL;
                 }
#if 1              
                if((strcmp(JsonCmd["cmd"], "HEART_BEAT") == 0))
                {
                       Response["cmd"] = "HEART_BEAT_RESPONSE";
                       Response["mac_address"] = DEVICE_ID;//modem.getIMEI();                                    
                       Response["success"] = 1;
                       Response["value"] = "ON";
                       Serial.println("*****************HEART_BEAT Recvd**********");  ;  
                    valid =1;
                }
                if(strcmp(JsonCmd["cmd"], "GET") == 0)
                {
                    Serial.println("*****************GET Recvd**********");
                    Response["cmd"] = "GET_RESPONSE";
                    Response["success"] = 1;
                    Response["mac_address"] = DEVICE_ID;//modem.getIMEI();
                    Response["value"]["ph"] = measuredParams.f_PHValue;
                    Response["value"]["ec"] = measuredParams.f_ECValue;
                    Response["value"]["temp"] = measuredParams.f_TempValue;
                    Response["value"]["lux"] = 0;
                    Response["value"]["motor_on_time"] = (measParams.waterPumpOnTime/3600);
                    Response["value"]["motor_off_time"] = (measParams.waterPumpOffTime/3600);
                    Response["value"]["lights_on_time"] = (measParams.lightPumpOnTime/3600);
                    Response["value"]["lights_off_time"] = (measParams.lightPumpOffTime/3600);
                    Response["value"]["dosage_ph_high_count"] = measParams.PH_dosingPump_highTime;
                    Response["value"]["dosage_ph_low_count"] = measParams.PH_dosingPump_lowTime;
                    Response["value"]["dosage_ec_high_count"] = measParams.EC_dosingPump_highTime;
                    Response["value"]["dosage_ec_low_count"] = measParams.EC_dosingPump_low_ATime;    
                    //Response["dosage_ec_lowA_count"] = measParams.EC_dosingPump_low_ATime;
                    //Response["dosage_ec_lowB_count"] = measParams.EC_dosingPump_low_BTime;
                    Response["mac_address"] = DEVICE_ID;//modem.getIMEI();
                    valid =1;
                }
                if(strcmp(JsonCmd["cmd"], "DOSE_ON") == 0)
                {
                    
                    Serial.println("*****************DOSE_ON Recvd**********");
                    //TBD to turn on/off dosage 
                    Response["cmd"] = "DOSE_ON_RESPONSE";
                    Response["success"] = 1;    
                    setParams.u8_DosageTurnOn = 1;
                    Response["mac_address"] = DEVICE_ID;//modem.getIMEI();
                    valid =1;
                }
                if(strcmp(JsonCmd["cmd"], "DOSE_OFF") == 0)
                {
                     Serial.println("*****************DOSE_OFF Recvd**********");
                    Response["cmd"] = "DOSE_OFF_RESPONSE";
                    Response["success"] = 1;      
                    setParams.u8_DosageTurnOn = 0;  
                    Response["mac_address"] = DEVICE_ID;//modem.getIMEI(); 
                    valid =1;           
                }
#ifdef DEMO_MODE                
                if(strcmp(JsonCmd["cmd"], "LIGHTS_ON") == 0)
                {
                     Serial.println("*****************LIGHTS_ON Recvd**********");
                    Response["cmd"] = "LIGHTS_ON_RESPONSE";
                    Response["success"] = 1;                          
                    Response["mac_address"] = DEVICE_ID;//modem.getIMEI(); 
                    valid =1;           
                    measParams.manuallighton = true;
                }
                if(strcmp(JsonCmd["cmd"], "LIGHTS_OFF") == 0)
                {
                     Serial.println("*****************LIGHTS_OFF Recvd**********");
                    Response["cmd"] = "LIGHTS_OFF_RESPONSE";
                    Response["success"] = 1;                          
                    Response["mac_address"] = DEVICE_ID;//modem.getIMEI(); 
                    valid =1;           
                    measParams.manuallighton = false;
                }
                if(strcmp(JsonCmd["cmd"], "WATER_ON") == 0)
                {
                     Serial.println("*****************WATER_ON Recvd**********");
                    Response["cmd"] = "WATER_ON_RESPONSE";
                    Response["success"] = 1;                          
                    Response["mac_address"] = DEVICE_ID;//modem.getIMEI(); 
                    valid =1;           
                    measParams.manualpumpon = true;
                }
                if(strcmp(JsonCmd["cmd"], "WATER_OFF") == 0)
                {
                     Serial.println("*****************WATER_OFF Recvd**********");
                    Response["cmd"] = "WATER_OFF_RESPONSE";
                    Response["success"] = 1;                          
                    Response["mac_address"] = DEVICE_ID;//modem.getIMEI(); 
                    valid =1;           
                    measParams.manualpumpon = false;
                }

                if(strcmp(JsonCmd["cmd"], "PH_INCREASE") == 0)
                {
                     Serial.println("*****************PH_INCREASE Recvd**********");
                    Response["cmd"] = "PH_INCREASE_RESPONSE";
                    Response["success"] = 1;                          
                    Response["mac_address"] = DEVICE_ID;//modem.getIMEI(); 
                    valid =1;           
                    measParams.f_PHValue +=5;                    
                }
                if(strcmp(JsonCmd["cmd"], "PH_DECREASE") == 0)
                {
                     Serial.println("*****************PH_DECREASE Recvd**********");
                    Response["cmd"] = "PH_DECREASE_RESPONSE";
                    Response["success"] = 1;      
                    Response["mac_address"] = DEVICE_ID;//modem.getIMEI(); 
                    valid =1;           
                    if( measParams.f_PHValue > 1 )
                        measParams.f_PHValue -= 5;                    
                    else
                        measParams.f_PHValue = 0.00;                    
                }
                
                if(strcmp(JsonCmd["cmd"], "EC_INCREASE") == 0)
                {
                     Serial.println("*****************EC_INCREASE Recvd**********");
                    Response["cmd"] = "EC_INCREASE_RESPONSE";
                    Response["success"] = 1;                          
                    Response["mac_address"] = DEVICE_ID;//modem.getIMEI(); 
                    valid =1;           
                    measParams.f_ECValue +=100;                     
                }
                if(strcmp(JsonCmd["cmd"], "EC_DECREASE") == 0)
                {
                     Serial.println("*****************EC_DECREASE Recvd**********");
                    Response["cmd"] = "EC_DECREASE_RESPONSE";
                    Response["success"] = 1;                          
                    Response["mac_address"] = DEVICE_ID;//modem.getIMEI(); 
                    valid =1;           
                     
                    if( measParams.f_ECValue > 10 )
                        measParams.f_ECValue -= 100;                    
                    else
                        measParams.f_ECValue = 0.00;                 
                }

#endif                
                if(strcmp(JsonCmd["cmd"], "UPDATE") == 0)
                {
                    Serial.println("*****************UPDATE Recvd**********");
                    Response["cmd"] = "UPDATE_RESPONSE";
                    Response["success"] = 1;                    
                    measuredParams.f_PHValue =  JsonCmd["ph"];
                    measuredParams.f_ECValue = JsonCmd["ec"];
                    measuredParams.f_TempValue = JsonCmd["temp"];
                    //JsonCmd["lux"];                    
                    measParams.waterPumpOnTime = JsonCmd["motor_on_time"];
                    measParams.waterPumpOffTime = JsonCmd["motor_off_time"];
                    measParams.lightPumpOnTime = JsonCmd["lights_on_time"];
                    measParams.lightPumpOffTime  = JsonCmd["lights_off_time"];
                    measParams.waterPumpOnTime *= 3600;
                    measParams.waterPumpOffTime *= 3600;
                    measParams.lightPumpOnTime *= 3600;
                    measParams.lightPumpOffTime  *= 3600;
                    measParams.PH_dosingPump_highTime  = JsonCmd["dosage_ph_high_count"];
                    measParams.PH_dosingPump_lowTime  = JsonCmd["dosage_ph_low_count"];
                    measParams.EC_dosingPump_highTime  = JsonCmd["dosage_ec_high_count"];
                    measParams.EC_dosingPump_low_ATime  = JsonCmd["dosage_ec_low_count"];
                    valid =1;           
                }
                if(strcmp(JsonCmd["cmd"], "SET") == 0)
                {
                    Serial.println("*****************SET Recvd**********");
                        setParams.f_PH_LowThreshold = JsonCmd["ph_low"];
                        setParams.f_PH_HighThreshold = JsonCmd["ph_high"];
                        setParams.f_EC_LowThreshold = JsonCmd["ec_low"];
                        setParams.f_EC_HighThreshold = JsonCmd["ec_high"];
                        setParams.f_Temp_LowThreshold = JsonCmd["temp_low"];
                        setParams.f_Temp_HighThreshold = JsonCmd["temp_high"];
                        //TODO Lux                                                                                                                                                                                                                                                                                           
                        //setParams.f_PH_LowThreshold = JsonCmd["lux_high"];
                        //setParams.f_PH_LowThreshold = JsonCmd["lux_high"];
                        //setParams. = JsonCmd["water_level"];
                        setParams.u32_PumpOnTime = JsonCmd["motor_on_time"];
                        setParams.u32_PumpOnTime *= 3600;
                        setParams.u32_PumpOffTime = JsonCmd["motor_off_time"];
                        setParams.u32_PumpOffTime *= 3600;
                        setParams.u32_LightsOnTime = JsonCmd["lights_on_time"];
                        setParams.u32_LightsOnTime *= 3600;
                        setParams.u32_LightsOffTime = JsonCmd["lights_off_time"];                        
                        setParams.u32_LightsOffTime *= 3600;
                        setParams.container_height = JsonCmd["container_height"];              
                        setParams.water_height = JsonCmd["water_height"];   
                        setParams.KitStart = 1;           

                        // uint32_t len;
                        // char periodic_check[20] = {0};                        
                        // strcpy(periodic_check,JsonCmd["periodic_check"]);
                        // len = strlen(periodic_check);
                        // periodic_check[len-1] = '\0';
                        // setParams.u32_MeasureFreqInSec = atoi(periodic_check);                        
                        // setParams.u32_MeasureFreqInSec *= 3600;
                        setParams.u32_MeasureFreqInSec = 5;

                        Response["mac_address"] = DEVICE_ID;//modem.getIMEI();
                        Response["cmd"] = "SET_RESPONSE";
                        Response["success"] = 1;  
                        valid =1;                                            
                }
                if( (Response["cmd"]!=NULL)&&(valid==1))
                {
                    serializeJson(Response, dataToSend);                   
                }
                char txpayload[1024] ={0};
                if((strlen(dataToSend) != 0)&&(valid ==1))
                {   
                    sprintf(txpayload,"%d#%s",strlen(dataToSend),dataToSend);
                    client.write(txpayload);                    
                    //client.write(dataToSend);
                    Serial.println("Sending Response");  
                    Serial.println(txpayload);  
                    Response["cmd"] = NULL;
                    //CommandRecvd.clear();
                    txpayload[0] = '\0';
                    dataToSend[0] = '\0';
                }
                dataToSend[0] = '\0';
#endif                
            }
                   
        }while(CommandRecvd.length() > (index+1));        
      }      
    }
        //Check for host reachability,GPRS connectivity and registration status
      if ((!client.connected())||(!modem.isGprsConnected())||(!((modem.getRegistrationStatus()==REG_OK_HOME)
      ||((modem.getRegistrationStatus()==REG_OK_ROAMING)))))
      {        
        digitalWrite(MODEM_POWER_SWITCH, HIGH);
        Serial.println("Modem Reset Switch");  
        GSM_CONNECT_OK = false;
        GSM_reconnect = true;
      }
//#ifdef MODEM_POWER_CONTROL   
	//TODO if modem not responding to AT due to hang
      if(!modem.testAT(100))
      {
           digitalWrite(MODEM_POWER_SWITCH, HIGH); //Power off modem
           Serial.println("Modem Reset Switch");  
           GSM_CONNECT_OK = false;
           GSM_reconnect = true;
      }
//#endif      
    }
 
   }
    void Modem::sendSMSAlert(char *phnum)
    {
    }
}