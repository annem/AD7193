/* blah blah */
#ifndef AD7193_h
#define AD7193_h

#include <Arduino.h>
#include <SPI.h>

#define AD7193_CS_PIN     10  // define the chipselect
#define AD7193_RDY_STATE  MISO   // pin to watch for data ready state

/* AD7193 Register Map */
#define AD7193_REG_COMM         0 // Communications Register (WO, 8-bit) 
#define AD7193_REG_STAT         0 // Status Register         (RO, 8-bit) 
#define AD7193_REG_MODE         1 // Mode Register           (RW, 24-bit 
#define AD7193_REG_CONF         2 // Configuration Register  (RW, 24-bit)
#define AD7193_REG_DATA         3 // Data Register           (RO, 24/32-bit) 
#define AD7193_REG_ID           4 // ID Register             (RO, 8-bit) 
#define AD7193_REG_GPOCON       5 // GPOCON Register         (RW, 8-bit) 
#define AD7193_REG_OFFSET       6 // Offset Register         (RW, 24-bit 
#define AD7193_REG_FULLSCALE    7 // Full-Scale Register     (RW, 24-bit)

/* Communications Register Bit Designations (AD7193_REG_COMM) */
#define AD7193_COMM_WEN         (1 << 7)           // Write Enable. 
#define AD7193_COMM_WRITE       (0 << 6)           // Write Operation.
#define AD7193_COMM_READ        (1 << 6)           // Read Operation. 
#define AD7193_COMM_ADDR(x)     (((x) & 0x7) << 3) // Register Address. 
#define AD7193_COMM_CREAD       (1 << 2)           // Continuous Read of Data Register.





class AD7193
{
public:
	AD7193(void); //need to add this function
	//bool begin(Stream &serialPort = Serial);  // does this help me specify serial?
	
	bool begin(void);
	void Reset(void);
	void Calibrate(void);

	void SetPGAGain(int gain);
	void SetAveraging(int filterRate);
	void SetChannel(int channel);
	
	void SetPsuedoDifferentialInputs(void);
	void AppendStatusValuetoData(void);
	
	unsigned long ReadADCChannel(int channel);
	unsigned long ReadADCData(void);
	void IntitiateSingleConversion(void);
	void WaitForADC(void);
	

	float DataToVoltage(long rawData);
	float TempSensorDataToDegC(unsigned long rawData);
	
	
	unsigned long GetRegisterValue(unsigned char registerAddress,
                                       unsigned char bytesNumber,
                                       unsigned char modifyCS);
	void SetRegisterValue(unsigned char registerAddress,
                              unsigned long registerValue,
                              unsigned char bytesNumber,
                              unsigned char modifyCS);

	
	void ReadRegisterMap(void);

	//void SetChannelSelect(unsigned long wordValue);
	//void ChannelEnable(int channel);
	
	//void SingleConversionAndReadADC(long unsigned int *ADCDataByChannel);  // this needs to be a pointer
	//void DisplayADCData(long unsigned int ADCDataByChannel[]);
	//void WriteAllRegisters(void);


private:

};

#endif