

/* blah blah */

#include <SPI.h>
#include <AD7193.h>

// I need this array to be accessible in the sketch - should it be in the library?
// if so, I was having trouble 
unsigned long ADCDataByChannel[10];

AD7193 AD7193;

void setup() {
  
  ///////////////////////////
  // setup Serial and SPI
  ///////////////////////////
  
  Serial.begin(9600);
  delay(1000);
  
  AD7193.begin();

  // Reset AD7193
  AD7193.Reset();
 
  // register check at initial reset
  AD7193.ReadRegisterMap();

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

  AD7193.SetAveraging(80);
  
  AD7193.ChannelEnable(0);  // Potentiometer connected to AIN1/AIN2
  AD7193.ChannelEnable(1);  // thermocouple connected to AIN3/AIN4
  //AD7193_ChannelEnable(2);
  //AD7193_ChannelEnable(3);

  AD7193.ChannelEnable(8);  // temperature
  AD7193.ChannelEnable(9);  // short
  
  // This will write all registers - might be helpful for debug
  //AD7193_WriteAllRegisters();
  
  // Check register map values
  AD7193.ReadRegisterMap();
  

  /////////////////////////////////////
  // Calibrate with given PGA settings - need to recalibrate if PGA setting is changed
  /////////////////////////////////////
  
  AD7193.IntitiateInternalCalibration();

  // Check register map values
  AD7193.ReadRegisterMap();
  

  //////////////////////////////////////
  
  Serial.println("\nBegin AD7193 conversion - single conversion (pg 35 of datasheet, figure 25)");
}


void loop() {

  // only works when status append is enabled
  AD7193.SingleConversionAndReadADC(ADCDataByChannel);

  AD7193.DisplayADCData(ADCDataByChannel);

  // Convert AD7193 data to temperature for IC Termerature channel (8)
  float ambientTemp = AD7193.BinaryToTemperatureDegC(ADCDataByChannel[8] >> 8);

  // Convert voltage to tmerpature for Channel 1 - with K-type thermocouple connected
  float thermocoupleTempRelative = Thermocouple_Ktype_VoltageToTempDegC(AD7193.BinaryToVoltage((ADCDataByChannel[1] >> 8)));

  // report Thermocouple measurement results
  Serial.print("\n\t\tChannel 1: ");
  Thermocouple_PrintResults(ambientTemp, thermocoupleTempRelative, "degC");
  
  delay(900);
}












