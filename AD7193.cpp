/*blah blah*/

#include <AD7193.h>

// default register settings
unsigned long registerMap[4] = {
  0x00,
  0x080060,
  0x000117,
  0x000000
};

int registerSize[8] = {
  1,
  3,
  3,
  3,
  1,
  1,
  3,
  3
};



AD7193::AD7193() {
  // Constructor
}

// need to fill this out
bool AD7193::begin(void) {
  SPI.begin();
  SPI.setDataMode(SPI_MODE3);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  pinMode(AD7193_RDY_STATE, INPUT_PULLUP);
  //or begin(Stream &serialPort)
  // setup serial?
  //LibrarySerial = &serialPort; //Grab which port the user wants us to use
}


void AD7193::Reset(void)  {
  Serial.println("\nReset Device...");

  digitalWrite(AD7193_CS_PIN, LOW);
  delay(100);

  char i = 0;
  for(i = 0; i < 6; i++)
  {
    SPI.transfer(0xFF);
  }    
  digitalWrite(AD7193_CS_PIN, HIGH);
  delay(100);
}

void AD7193::SetPGAGain(int gain)  {

  Serial.print("\nSetting PGA Gain to ");
  Serial.println(gain);
  
  unsigned long gainSetting;

  if(gain == 1)         {gainSetting = 0x0;}
  else if (gain == 8)   {gainSetting = 0x3;}
  else if (gain == 16)  {gainSetting = 0x4;}
  else if (gain == 32)  {gainSetting = 0x5;}
  else if (gain == 64)  {gainSetting = 0x6;}
  else if (gain == 128) {gainSetting = 0x7;}
  else {
    Serial.println("\tERROR - Invalid Gain Setting - no changes made.  Valid Gain settings are 1, 8, 16, 32, 64, 128");
    return;
  }

  int regAddress = AD7193_REG_CONF;

  registerMap[regAddress] &= 0xFFFFF8; //keep all bit values except gain bits
  registerMap[regAddress] |= gainSetting;

  SetRegisterValue(regAddress, registerMap[regAddress], registerSize[regAddress], 1);
}

void AD7193::SetAveraging(int filterRate)  {

  Serial.print("\nSetting Filter Rate Select Bits to ");
  Serial.println(filterRate);

  if(filterRate > 0x3ff) {
    Serial.println("\tERROR - Invalid Filter Rate Setting - no changes made.  Filter Rate is a 10-bit value");
    return;
  }

  registerMap[1] &= 0xFFFC00; //keep all bit values except filter setting bits
  registerMap[1] |= filterRate;

  SetRegisterValue(1, registerMap[1], registerSize[1], 1);

  //Serial.print(" on next register refresh - New Mode Reg value will be: ");
  //Serial.println(registerMap[1], HEX);
  //Serial.println(); 
}

void AD7193::SetChannelSelect(unsigned long wordValue)  {

  Serial.print("\nSetting Channel Select Word to ");
  Serial.println(wordValue, HEX);
  
  registerMap[2] &= 0xFC00FF; //keep all bit values except Channel bits
  registerMap[2] |= wordValue << 8;

  SetRegisterValue(2, registerMap[2], registerSize[2], 1);
  
  //Serial.print(" on next register refresh - New Config Reg value will be: ");
  //Serial.println(registerMap[2], HEX); 
}

void AD7193::ChannelEnable(int channel)  {
  Serial.print("\nEnabling Channel ");
  Serial.println(channel);

  unsigned long shiftvalue = 0x00000100;
  shiftvalue = shiftvalue << channel;
  registerMap[2] |= shiftvalue;

  SetRegisterValue(2, registerMap[2], registerSize[2], 1);

  //Serial.print(" - on next register refresh, new Config Reg value will be: ");
  //Serial.println(registerMap[2], HEX);
}

void AD7193::SetPsuedoDifferentialInputs(void)  {
  Serial.println("Switching from differential input to pseudo differential inputs...");
  
  unsigned long psuedoBit = 0x040000;
  registerMap[2] |= psuedoBit;

  SetRegisterValue(2, registerMap[2], registerSize[2], 1);

  //Serial.print(" - on next register refresh, new Config Reg value will be: ");
  //Serial.println(registerMap[2], HEX);
}

void AD7193::AppendStatusValuetoData(void) {
  Serial.println("\nEnabling DAT_STA Bit (appends status register to data register when reading");


  registerMap[1] &= 0xEFFFFF; //keep all bit values except DAT_STA bit
  registerMap[1] |= 0x100000;  // set DAT_STA to 1

  SetRegisterValue(1, registerMap[1], registerSize[1], 1);
  
  //Serial.print(" - New Mode Reg Value: ");
  //Serial.println(registerMap[1], HEX);

  registerSize[3] = 4; // change register size to 4, b/c status register is now appended
}

void AD7193::IntitiateInternalCalibration(void) {
  Serial.println("\nInitiate Internal Calibration, starting with Zero-scale calibration...");

  // Begin Communication cycle, bring CS low manually
  digitalWrite(AD7193_CS_PIN, LOW);
  delay(100);
  
  registerMap[1] &= 0x1FFFFF; //keep all bit values except Channel bits
  registerMap[1] |= 0x800000; // internal zero scale calibration

  SetRegisterValue(1, registerMap[1], 3, 0);  // overwriting previous MODE reg setting 

  WaitRdyGoLow();

  Serial.println("\nNow full-scale calibration...");


  registerMap[1] &= 0x1FFFFF; //keep all bit values except Channel bits
  registerMap[1] |= 0xA00000; // internal full scale calibration

  SetRegisterValue(1, registerMap[1], 3, 0);  // overwriting previous MODE reg setting 

  WaitRdyGoLow();

  digitalWrite(AD7193_CS_PIN, HIGH);
  delay(100); 
}

void AD7193::WaitRdyGoLow(void)  {
    int breakTime = 0;

    while(1){
      if (digitalRead(AD7193_RDY_STATE) == 0){      // Break if ready goes low
        break;
      }

      if (breakTime > 5000) {                       // Break after five seconds - avoids program hanging up
        Serial.print("Data Ready never went low!  Make sure Arduino pin ");
        Serial.print(AD7193_RDY_STATE);
        Serial.println(" is connected to AD7193 DOUT pin");
        break;
      }

      delay(1);
      breakTime = breakTime + 1;
    }
}

unsigned long AD7193::ReadADCData(void)  {
  
    unsigned char byteIndex = 0;
    unsigned long buffer = 0;
    unsigned char receiveBuffer = 0;
    unsigned char dataLength = registerSize[3];

    SPI.transfer(0x58);  // command to start read data
    
    while(byteIndex < dataLength)
    {
      receiveBuffer = SPI.transfer(0);
      buffer = (buffer << 8) + receiveBuffer;
      byteIndex++;
    }

    return(buffer);
}

void AD7193::SingleConversionAndReadADC(long unsigned int *ADCDataByChannel)  {

  unsigned long ADCDataBuffer[10];
  int NumOfChannels = 0;
  unsigned long channelSetting = (registerMap[2] & 0x03FF00) >> 8;
 
  // tally up number of channels enabled, and clear ADCDataByChannel array
  for(int i = 0; i < 10; i++) {
    NumOfChannels += bitRead(channelSetting, i);
    ADCDataByChannel[i] = 0x0;
  }

  // initiate, single conversion - Set Mode Register per Page 35 of data sheet
  IntitiateSingleConversion();

  // grab channel data, looping for each channel that's enabled
  for(int i = 0; i < NumOfChannels; i++) {

    WaitRdyGoLow();  // wait for ADC conversion to complete - Dout pin will go low
    
    ADCDataBuffer[i] = ReadADCData();  // read data
    delay(10); 

    // sort the data into an array with location = channel number
    int channelNumber = ADCDataBuffer[i] & 0xFF;
    ADCDataByChannel[channelNumber] = ADCDataBuffer[i];
  }

  // end communication cycle, bringing CS pin High manually 
  digitalWrite(AD7193_CS_PIN, HIGH);
  delay(100);
}

void AD7193::IntitiateSingleConversion(void) {
  Serial.print("    Initiate Single Conversion... (Device will go into low pwer mode when conversion complete)");

  // Begin Communication cycle, bring CS low manually
  digitalWrite(AD7193_CS_PIN, LOW);
  delay(100);
  
  registerMap[1] &= 0x1FFFFF; //keep all bit values except Channel bits
  registerMap[1] |= 0x200000; // single conversion mode bits  

  SetRegisterValue(1, registerMap[1], 3, 0);  // overwriting previous MODE reg setting 
}

float AD7193::BinaryToVoltage(long rawData)  {
  float voltage = 0;
  char mGain = 0;
  float mVref = 2.5;
  char mPolarity = 0;

  int PGASetting = registerMap[2] & 0x000007;  // keep only the PGA setting bits
  int PGAGain;

  if (PGASetting == 0) {
    PGAGain = 1;
  } else if (PGASetting == 3) {
    PGAGain = 8;
  } else if (PGASetting == 4) {
    PGAGain = 16;
  } else if (PGASetting == 5) {
    PGAGain = 32;
  } else if (PGASetting == 6) {
    PGAGain = 64;
  } else if (PGASetting == 7) {
    PGAGain = 128;
  } else {
    PGAGain = 1;
  }

  
  //Serial.print("PGA Gain = ");
  //Serial.println(PGAGain);

  
  if(mPolarity == 1)
  {
    voltage = ((double)rawData / 16777216 / (1 << PGAGain)) * mVref; 
  }
  if(mPolarity == 0)
  {
    voltage = (((float)rawData / (float)8388608) - (float)1) * (mVref / (float)PGAGain);
  }


  return(voltage);
}

float AD7193::BinaryToTemperatureDegC(unsigned long rawData)  {
        float degC = (float(rawData - 0x800000) / 2815) - 273;
        float degF = (degC * 9 / 5) + 32;
        /*Serial.print(degC);
        Serial.print(" degC, ");
        Serial.print(degF);
        Serial.print(" degF\t");*/
        return(degC);
}

void AD7193::DisplayADCData(long unsigned int ADCDataByChannel[]) {

  unsigned long channelSetting = (registerMap[2] & 0x03FF00) >> 8;

  Serial.print("\n\t\t\t\t\t");

  for(int i = 0; i < 10; i++) {
    if (bitRead(channelSetting, i) == 1) {
      Serial.print("CH");
      //Serial.print(ADCDataByChannel[i] & 0xFF);
      Serial.print(i);
      Serial.print("\t\t");
    }
  }

  Serial.print("\n\t\tRaw Data Received:\t");
  
  for(int i = 0; i < 10; i++) {
    if (bitRead(channelSetting, i) == 1) {
      Serial.print((ADCDataByChannel[i] >> 8), HEX);
      Serial.print(",\t\t");
    }
  }

  Serial.print("\n\t\tData in Volts:\t\t");
  
  for(int i = 0; i < 10; i++) {
    if (bitRead(channelSetting, i) == 1) {
      Serial.print(BinaryToVoltage(ADCDataByChannel[i] >> 8), 5);
      Serial.print(" V,\t");
    }
  }

  Serial.println();
}

/*! Reads the value of a register. */
unsigned long AD7193::GetRegisterValue(unsigned char registerAddress, unsigned char bytesNumber, unsigned char modifyCS)  {//getregistervalue
    
    unsigned char receiveBuffer = 0;
    unsigned char writeByte = 0;
    unsigned char byteIndex = 0;
    unsigned long buffer = 0;
    
    writeByte = AD7193_COMM_READ | AD7193_COMM_ADDR(registerAddress);
    if(modifyCS == 1)
    {
      digitalWrite(AD7193_CS_PIN, LOW);
    }

    SPI.transfer(writeByte);
    while(byteIndex < bytesNumber)
    {
        receiveBuffer = SPI.transfer(0);
        buffer = (buffer << 8) + receiveBuffer;
        byteIndex++;
    }

    if(modifyCS == 1)
    {
      digitalWrite(AD7193_CS_PIN, HIGH);
    }

    char str[32];
    sprintf(str, "%06x", buffer);

    Serial.print("    Read Register Address: ");
    Serial.print(registerAddress, HEX);
    Serial.print(", command: ");
    Serial.print(writeByte, HEX);
    Serial.print(", recieved: ");
    Serial.print(buffer, HEX);
    Serial.print(" - ");
    Serial.println(str);
    
    
    return(buffer);
}

/*! Writes data into a register. */
void AD7193::SetRegisterValue(unsigned char registerAddress,  unsigned long registerValue,  unsigned char bytesNumber,  unsigned char modifyCS)  {//setregistervalue
    unsigned char commandByte = 0;
    unsigned char txBuffer[4] = {0, 0, 0 ,0};
    
    commandByte = AD7193_COMM_WRITE | AD7193_COMM_ADDR(registerAddress);
  
    txBuffer[0] = (registerValue >> 0)  & 0x000000FF;
    txBuffer[1] = (registerValue >> 8)  & 0x000000FF;
    txBuffer[2] = (registerValue >> 16) & 0x000000FF;
    txBuffer[3] = (registerValue >> 24) & 0x000000FF;
    if(modifyCS == 1)
    {
      digitalWrite(AD7193_CS_PIN, LOW);
    }

    SPI.transfer(commandByte);
    while(bytesNumber > 0)
    {
        SPI.transfer(txBuffer[bytesNumber - 1]);
        bytesNumber--;
    }
    if(modifyCS == 1)
    {
      digitalWrite(AD7193_CS_PIN, HIGH);
    }

    Serial.print("    Write Register Address: ");
    Serial.print(registerAddress, HEX);
    Serial.print(", command: ");
    Serial.print(commandByte, HEX);
    Serial.print(",     sent: ");
    Serial.println(registerValue, HEX);
}

void AD7193::ReadRegisterMap(void)  {
  Serial.println("\nRead All Register Values");
  GetRegisterValue(0, 1, 1);
  GetRegisterValue(1, 3, 1);
  GetRegisterValue(2, 3, 1);
  GetRegisterValue(3, registerSize[3], 1);
  GetRegisterValue(4, 1, 1);
  GetRegisterValue(6, 3, 1);
  GetRegisterValue(7, 3, 1);
  delay(100);
}

void AD7193::WriteAllRegisters(void) {
  Serial.println("\nWriting all new register values to registers...");
  SetRegisterValue(1, registerMap[1], registerSize[1], 1);
  SetRegisterValue(2, registerMap[2], registerSize[2], 1);
  SetRegisterValue(3, registerMap[3], registerSize[3], 1);
  SetRegisterValue(4, registerMap[4], registerSize[4], 1);
  delay(100);
}
