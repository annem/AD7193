/*

AD7193_Thermocouple_Example

Configures and calibrates the AD7193 to take K-type thermocouple measurements on channel 0.  
Performs cold juntion compensation using the embedded temperature sensor on the AD7193.

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

---------------------------------------------------------
 AD7980 Signal Name     |  PmodAD5 Pin  |  Thermocouple
---------------------------------------------------------
  AIN1                  |  J2, Pin 1    |  Pos lead
  AIN2                  |  J2, Pin 2    |  Neg lead
---------------------------------------------------------
if these are backwards, hot temps will be measured as cold, and vice-versa.
Just swap the connections if this happens

See Table 24 of AD7193 datasheet for more information

Reference:
https://reference.digilentinc.com/reference/pmod/pmodad5/start
https://www.arduino.cc/en/Reference/SPI

Created 4 Oct 2016
by Anne Mahaffey

This example code is in the public domain
*/







// include SPI and AD7193 libraries
#include <SPI.h>
#include <AD7193.h>

AD7193 AD7193;



void setup() {
  
  ///////////////////////////
  // setup Serial and SPI
  ///////////////////////////
  
  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  delay(1000);
  
  AD7193.begin();


  ///////////////////////////////////
  // Device setup
  ///////////////////////////////////
  
  //This will append status bits onto end of data - is required for library to work properly
  AD7193.AppendStatusValuetoData();  
  
  // Set the gain of the PGA
  AD7193.SetPGAGain(8);

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

  unsigned long ch0Data;
  unsigned long chTempData;
  
  
  // Read channel measurement data
  ch0Data = (AD7193.ReadADCChannel(0) >> 8);     // last 8 bits of ch0Data are status bits, and not data bits
  Serial.print("  CH0 data: ");
  Serial.print(ch0Data, HEX);
  
  // Read on-chip temperature data
  chTempData = (AD7193.ReadADCChannel(8) >> 8);   // last 8 bits of ch0Data are status bits, and not data bits
  Serial.print("  CH8 data (temperature): ");
  Serial.println(chTempData, HEX);         
  
  // Convert AD7193 temperature data to temperature (degC), 
  // and calculate equivalent voltage - used for cold junction compensation
  float ambientTemp = AD7193.TempSensorDataToDegC(chTempData);
  float referenceVoltage = Thermocouple_Ktype_TempToVoltageDegC(ambientTemp);

  // measure thermocouple voltage
  float thermocoupleVoltage = AD7193.DataToVoltage(ch0Data);

  // Cold Junction Compensation
  float compensatedVoltage = thermocoupleVoltage + referenceVoltage;
  float compensatedTemperature = Thermocouple_Ktype_VoltageToTempDegC(compensatedVoltage);

  // Display Summary
  Serial.print("\n\t\tChannel 0 Compensated Thermocouple Voltage Measurement: ");
  Serial.print(compensatedTemperature, 3);  Serial.println(" degC");
  Serial.print("\t\t--------------------------------------------------------");
  Serial.println("\n\t\tThermocouple Measurement Details:");
  Serial.print("\t\t\tThermocouple Voltage: ");  Serial.println(thermocoupleVoltage, 7);
  Serial.print("\t\t\tReference Temp: ");  Serial.println(ambientTemp, 5);
  Serial.print("\t\t\tReference Temp Voltage Equivalent: ");  Serial.println(referenceVoltage, 5);
  Serial.println("");
  
  delay(100);
}