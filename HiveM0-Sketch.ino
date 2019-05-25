//###################################################
// Include the libraries we need

#include "OneWire.h"
#include "DallasTemperature.h"
#include "ADS1231.h"
#include "Sodaq_nbIOT.h"
#include "Sodaq_wdt.h"
#include "FlashStorage.h"
#include "EasyHive.h"

extern Sodaq_nbIOT nbiot;
// Server variables
// define Server variables
extern const char* apn;
extern const char* cdp;
extern unsigned char cid;
extern const char* forceOperator; // optional - depends on SIM / network

/*
 * The setup function. We only start the sensors here and initialize the board state
 */

// Reserve a portion of flash memory to store an "bool" variable
// and call it "calibrated".
//FlashStorage(test, float);
 
void setup(void)
{
  sodaq_wdt_safe_delay(STARTUP_DELAY);
  
  init_LEDs();
  LEDs_red();
  delay(500);
  LEDs_green();
  delay(500);
  LEDs_blue();
  delay(500);
  LEDs_off();
    
  init_Temp();

  init_Weight();

  set_Weight_calib(8388607.0, 4.2);

  set_Weight_calib(12345.0, 42.0);

  //test.write(1.23);

  //enable_USB_in();

  /****** nbiot init *******/

  DEBUG_STREAM.begin(DEBUG_STREAM_BAUD);
  DEBUG_STREAM.println("Initializing and connecting... ");

  MODEM_STREAM.begin(nbiot.getDefaultBaudrate());
  nbiot.setDiag(DEBUG_STREAM);
  nbiot.init(MODEM_STREAM, powerPin, enablePin, -1, cid); 

  if (!nbiot.connect(apn, cdp, forceOperator, 8)) {
      DEBUG_STREAM.println("Failed to connect to the modem!");
  }

  //send startup message // ToDO: sendmessageUDP function variable übergeben können
  //String 
  sendMessageThroughUDP("EASYHIVE STARTUP");
    
}

/*
 * Main function, get and show the temperature, weight and sleep, send data via NB-IoT
 */
void loop(void)
{ 
  sodaq_wdt_safe_delay(5000);
  
  float temp_val = get_Temp();
  SerialUSB.print("Temp: ");
  SerialUSB.println(temp_val);
  delay(500);

  float weight_val = get_Weight();  
  SerialUSB.print("Weight: ");
  SerialUSB.println(weight_val);
  delay(500);

  weight_val = get_Weight_raw();  
  SerialUSB.print("Weight Raw: ");
  SerialUSB.println(weight_val);
  delay(500);

  float w_calib;   
  float w_offset; 
  get_Weight_calib(&w_calib, &w_offset);
  
  SerialUSB.println(w_calib);
  SerialUSB.println(w_offset); 
  
  SerialUSB.println("");  
  
  if (!nbiot.isConnected()) {
        if (!nbiot.connect(apn, cdp, forceOperator, 8)) {
            DEBUG_STREAM.println("Failed to connect to the modem!");
        }
  }
  else {
      
      const char* msg;
      msg = sendMessageThroughUDP("test");
      checkMessage(msg);
  }
}
  
