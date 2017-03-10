/*

AD7193_VoltageMeasure_Example

Configures and calibrates the AD7193 to measure differental voltage on AIN1 and AIN2

Tutorial can be found at <TBD>, and uses the PmodAD5 from Digilent.

Signal Connections:
-----------------------------------------------------
 Signal Name |  PmodAD Pin  |  Arduino Pin 
-----------------------------------------------------
  ~CS        |  J1, Pin 1   |  10
  MOSI       |  J1, Pin 2   |  MOSI or 11 or ICSP-4
  MISO       |  J1, Pin 3   |  MISO or 12 or ICSP-1
  SCLK       |  J1, Pin 4   |  SCLK or 13 or ICSP-3
  GND        |  J1, Pin 5   |  GND
  VCC (3.3V) |  J1, Pin 6   |  3.3V
-----------------------------------------------------

-----------------------------------
 Signal Name       |  PmodAD5 Pin    
-----------------------------------
 Channel 1 Pos     |  J2, Pin 3
 Channel 1 Neg     |  J2, Pin 4
-----------------------------------
See Table 24 of AD7193 datasheet for more information

Reference:
https://reference.digilentinc.com/reference/pmod/pmodad5/start
https://www.arduino.cc/en/Reference/SPI

Created 4 Oct 2016
by Anne Mahaffey

This example code is in the public domain
*/






#include <SPI.h>
#include <AD7193.h>


AD7193 AD7193;

void setup() {
  
  ///////////////////////////
  // setup Serial and SPI
  ///////////////////////////
  
  Serial.begin(9600);
  delay(1000);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  AD7193.begin();
  

  ///////////////////////////////////
  // Device setup
  ///////////////////////////////////
  
  //This will append status bits onto end of data - is required for library to work properly
  AD7193.AppendStatusValuetoData();  
  
  // Set the gain of the PGA
  AD7193.SetPGAGain(1);

  // Set the Averaging
  AD7193.SetAveraging(100);
  

  /////////////////////////////////////
  // Calibrate with given PGA settings - need to recalibrate if PGA setting is changed
  /////////////////////////////////////
  
  AD7193.Calibrate();

  // Debug - Check register map values
  AD7193.ReadRegisterMap();

  //////////////////////////////////////
  
  Serial.println("\nBegin AD7193 conversion - single conversion (pg 35 of datasheet, figure 25)");
}


void loop() {

  unsigned long ch1Data;
  float ch1Voltage;
  
  // Read channel measurement data
  ch1Data = (AD7193.ReadADCChannel(1) >> 8);

  Serial.print("  CH1 data: ");
  Serial.print(ch1Data, HEX);

  // Convert to voltage
  ch1Voltage = AD7193.DataToVoltage(ch1Data);
  
  Serial.print("\n\t\tChannel 1 Voltage Measurement: ");
  Serial.println(ch1Voltage, 3);  
   
  delay(100);
}








