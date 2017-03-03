

/* blah blah */

#include <SPI.h>
#include <AD7193.h>


AD7193 AD7193;

void setup() {
  
  ///////////////////////////
  // setup Serial and SPI
  ///////////////////////////
  
  Serial.begin(9600);
  delay(1000);
  
  AD7193.begin();

  ////////////////////////////////////////////
  // add function for setting up thermocouple?
  // Would need a thermocouple_tempToVoltage function to determine min/max voltage to measure
  // Would need a PGS Setup function that could calculate Gain factor, given a min/max voltage range value
  ////////////////////////////////////////////


  ///////////////////////////////////
  // Device setup
  ///////////////////////////////////
  
  AD7193.AppendStatusValuetoData();  //This might be a requirement - functions will be simpler if I can assume this is true  
  
  AD7193.SetPGAGain(8);

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
  
  chTempData = (AD7193.ReadADCChannel(8) >> 8);   // last 8 bits of ch0Data are status bits, and not data bits
  Serial.print("  CH8 data (temperature): ");
  Serial.println(chTempData, HEX);         
  

  // Convert AD7193 temperature data to temperature, and calculate equivalent voltage, for CJC
  float ambientTemp = AD7193.TempSensorDataToDegC(chTempData);
  float referenceVoltage = Thermocouple_Ktype_TempToVoltageDegC(ambientTemp);

  // measure thermocouple voltage
  float thermocoupleVoltage = AD7193.DataToVoltage(ch0Data);

  // Cold Junction Compensation
  float compensatedVoltage = thermocoupleVoltage + referenceVoltage;
  float compensatedTemperature = Thermocouple_Ktype_VoltageToTempDegC(compensatedVoltage);

  // Summary
  Serial.print("\n\t\tChannel 0 Compensated Thermocouple Voltage Measurement: ");
  Serial.print(compensatedTemperature, 3);  Serial.println(" degC");
  Serial.print("\t\t--------------------------------------------------------");
  Serial.println("\n\t\tThermocouple Measurement Details:");
  Serial.print("\t\t\tThermocouple Voltage: ");  Serial.println(thermocoupleVoltage, 7);
  Serial.print("\t\t\tReference Temp: ");  Serial.println(ambientTemp, 5);
  Serial.print("\t\t\tReference Voltage: ");  Serial.println(referenceVoltage, 5);
  Serial.println("");
  
  delay(100);
}