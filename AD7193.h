/* blah blah */

#include "Arduino.h"

#ifndef AD7193_h
#define AD7193_h

class AD7193
{
public:
	AD7193(void); //need to add this function
	//bool begin(Stream &serialPort = Serial);  // does this help me specify serial?

	void Reset(void);

	void SetPGAGain(int gain);
	void SetAveraging(int filterRate);
	void SetChannelSelect(unsigned long wordValue);
	void ChannelEnable(int channel);
	void SetPsuedoDifferentialInputs(void);
	void AppendStatusValuetoData(void);

	void IntitiateInternalCalibration(void);
	void WaitRdyGoLow(void);
	unsigned long ReadADCData(void);
	void SingleConversionAndReadADC(long unsigned int *ADCDataByChannel);  // this needs to be a pointer
	void IntitiateSingleConversion(void);

	float BinaryToVoltage(long rawData);
	float BinaryToTemperatureDegC(unsigned long rawData);
	void DisplayADCData(long unsigned int ADCDataByChannel[]);
	
	unsigned long GetRegisterValue(unsigned char registerAddress,
                                       unsigned char bytesNumber,
                                       unsigned char modifyCS);
	void SetRegisterValue(unsigned char registerAddress,
                              unsigned long registerValue,
                              unsigned char bytesNumber,
                              unsigned char modifyCS);

	void WriteAllRegisters(void);
	void ReadRegisterMap(void);


private:

};

#endif