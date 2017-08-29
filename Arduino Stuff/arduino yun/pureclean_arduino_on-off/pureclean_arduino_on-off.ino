///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
//  AGENT CONFIGURATION PARAMETERS  ( You only need define this parameters )
//  IMPORTANT: this agent version is only supported on devices ID from NB100246 onwards
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
char deviceId []    = "NB10XXXX";                                   // Put here the device_ID generated by the NearHub ( NB1xxxxx )
char sharedSecret[] = "XXXXXXXX";                             // The same as you configured in the NearHub (8 characters/number)  

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  

///////////////////////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////////////////////
#include <NearbusYun_v15.h>											
#include <Bridge.h>
//#include <YunClient.h>
#include <BridgeClient.h>																					
#include <HttpClient.h>
#include <SPI.h>
//#include <Servo.h>
#include <FlexiTimer2.h>
#include <IRLibSendBase.h>
#include <IRLib_HashRaw.h>



IRsendRaw mySender;
//pureclean robotic vacuum on/off
#define RAW_DATA_LEN 18
  uint16_t rawData[RAW_DATA_LEN]={
  2982, 1046, 686, 1574, 1398, 818, 1422, 790, 
  1426, 790, 634, 1570, 1454, 790, 662, 1574, 
  1426, 1000};

//pureclean robotic vacuum home
#define RAW_DATA_LEn 54
uint16_t rawdata[RAW_DATA_LEn]={
  2950, 1098, 662, 1570, 1426, 794, 630, 1570, 
  1426, 790, 662, 1570, 662, 1598, 1398, 814, 
  1398, 5226, 2978, 1074, 634, 1594, 1402, 786, 
  666, 1594, 1426, 786, 638, 1598, 662, 1570, 
  1398, 786, 1426, 5226, 2950, 1098, 638, 1598, 
  1398, 814, 714, 1518, 1398, 818, 634, 1598, 
  638, 1570, 1474, 762, 1398, 1000};


///////////////////////////////////////////////////////////////////////////////////////////
//  Global Variables               
/////////////////////////////////////////////////////////////////////////////////////////// 
Nearbus Agent(0);

ULONG A_register[8];                                                // Define the Tx Buffer (Reg_A)
ULONG B_register[8];                                                // Define the Rx Buffer (Reg_B) 
int retorno;

void AuxPortServices(void) {
    Agent.PortServices(); 
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
//  BROWSER API REST COMMAND LINE (for JavaScript) - (use it for troubleshooting)
//
//  http://nearbus.net/v1/api_vmcu_jsb/NB100***?user=****&pass=****&channel=0&service=DIG_OUTPUT&value=1&method=POST&reqid=123456
//
//  user:          Your NearBus Web user
//  pass:          Your NearBus Web password
//  channel:       NearBus channel [0-3]
//  value:         Service value (if apply)
//  method:        GET (read) / POST (write)
//  reqid:         Transaction identifier (to match a request and its response)
// 
//  SUPPORTED API SERVICES
//  DIG_INPUT:     Digital Input - Input Range [0-1] - Method: GET 
//  DIG_OUTPUT:    Digital Output - Output Range [0-1] - Method: POST/GET 
//  ADC_INPUT:     ADC Analog Input - Output Range [0-1023] - Method: GET 
//  PULSE _OUTPUT: Digital Output - Input Range [0-65535] in steps of 10ms (max 655 seg) - Method: POST/GET 
//  PWM_OUTPUT:    PWM Output calibrated for Servomotors - Input Range [800-2200] - Method: POST/GET   
//  DIG_COUNTER:   Pulse Counter / Accumulator
//  RMS_INPUT:     True RMS Meter
//  MY_NBIOS_0:    User defined function
//
//  For a detailed information please go to: http://goo.gl/Gxrcua
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  


/*####################################################################################################################################
#######################################################################################################################################                                                                                                                                       ###
###          MY_NBIOS CUSTOM FUNCTION CODE                                                                                        ####                                                                                                     ###                                                                                                                               ###
#######################################################################################################################################
 ####################################################################################################################################*/ 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
//  MOISTURE SENSOR - EXAMPLE OF CUSTOM CODE
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Product Web: http://www.seeedstudio.com/depot/grove-moisture-sensor-p-955.html
//  Wiki: http://seeedstudio.com/wiki/Grove_-_Moisture_Sensor
//  Code Source: n/a
//  Technical Spec: Humidytly: 5% RH - 99% RH - Temperature -40°C - 80°C - Acuracy: 2% RH / 0.5°C
//  Support Shield: Base Shield V1.3 - Grove compatible - http://seeedstudio.com/depot/base-shield-v13-p-1378.html
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
void Nearbus::MyNbios_0( byte portId, ULONG setValue, ULONG* pRetValue, byte vmcuMethod, PRT_CNTRL_STRCT* pPortControlStruct )
{
  
    //************************************
    // Reconfiguring Ports as I/O
    //************************************
    if( pPortControlStruct->portMode != MYNBIOS_MODE ) 
    {
	    PortModeConfig( portId, MYNBIOS_MODE );        
    }

    //************************************
    // Custom Function
    //************************************
    // DEFAULT:      The default analog reference of 5 volts (on 5V Arduino boards) or 3.3 volts (on 3.3V Arduino boards)
    // INTERNAL:     An built-in reference, equal to 1.1 volts on the ATmega168 or ATmega328 and 2.56 volts on the ATmega8 (not available on the Arduino Mega)
    // INTERNAL1V1:  A built-in 1.1V reference (Arduino Mega only)
    // INTERNAL2V56: A built-in 2.56V reference (Arduino Mega only)
    // EXTERNAL:     The voltage applied to the AREF pin (0 to 5V only) is used as the reference. 	
	
	
    analogReference( DEFAULT );	
       
    *pRetValue = (ULONG) analogRead( pPortControlStruct->anaPinId );   
  
}



/*####################################################################################################################################
#######################################################################################################################################
###          END OF CUSTOM CODE                                                                                                    ####                                                                                                     ###
#######################################################################################################################################
 ####################################################################################################################################*/ 

 

////////////////////////////////////////////////////////////////////// 
//   SETUP ROUTINE                                                                                 
//////////////////////////////////////////////////////////////////////                            
void setup(void)
{
         
    //*********************************
    // SERIAL INTERFACE INITIALIZATION
    //*********************************
    Serial.begin(9600);                                           // Start serial library
    Bridge.begin();
    HttpClient client;
    

    //*********************************
    // NEARBUS INITIALIZATION
    //*********************************  
    Agent.NearInit( deviceId, sharedSecret );

    //*********************************
    // FLEXITIMER INITIALIZATION
    //*********************************
	#if FLEXI_TIMER		
	    FlexiTimer2::set( INT_PERIOD, AuxPortServices );          // Call the port services routine every 10 ms
        FlexiTimer2::start();
	#endif     
}

void loop()
{
  HttpClient client;
int ret; 
        
    Agent.NearChannel( A_register, B_register, &ret );

    if ( ret >= 50 )
    {
        //Serial.println( "Rx Error" ); 
        // [50]  Frame Autentication Mismatch
        // [51]  Frame Out of sequence
        // [52]  Remote ACK Error
        // [53]  Unsupported Command    
    }
 
if( B_register[0] == 1){ 
        
        mySender.send(rawData,RAW_DATA_LEN,36);
        //Serial.println ("It Works!");
        delay(1000);
        //digitalWrite( 3, HIGH );
        client.get("http://nearbus.net/v1/api_transp/NB10XXXX?user=XXXXXXX&pass=XXXXXX&method=POST&reg00=0");
        while (client.available()) {
        char c = client.read();
        Serial.print(c);
  }
  Serial.flush();
  delay(2000);
    }
if( B_register[1] == 1){ 
        
        mySender.send(rawdata,RAW_DATA_LEn,36);
        //Serial.println ("It Works!");
        delay(1000);
        //digitalWrite( 3, HIGH );
        client.get("http://nearbus.net/v1/api_transp/NB10XXXX?user=XXXXXXX&pass=XXXXXX&method=POST&reg01=0");
        while (client.available()) {
        char c = client.read();
        Serial.print(c);
  }
  Serial.flush();
  delay(2000);
    }


}
