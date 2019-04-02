#ifndef INC_GS_INTERFACE_H
#define INC_GS_INTERFACE_H


// Define ADXL register map.  Each register identifier is followed by identifies for the
// bits of interest

#define GS_DEVID 			0x00	// Read-only

#define GS_BW_RATE 			0x2C	// Read-write
#define GS_POWER_CTL 		0x2D	// Read-write
#define MEASURE_BIT			0x08

#define GS_INT_SOURCE 	0x30	// Read-only
#define DATA_READY_BIT		0x80	

#define GS_DATA_FORMAT 	0x31	// Read-write
#define SELF_TEST_BIT		0x80
#define SPI_BIT				0x40
#define INT_INVERT_BIT		0x20
#define FULL_RES_BIT		0x08
#define JUSTIFY_BIT			0x04
#define RANGE_pm_2G			0x00
#define RANGE_pm_4G			0x01
#define RANGE_pm_8G			0x02
#define RANGE_pm_16G		0x03		

#define GS_DATAX0 		0x32	// Read-only
#define GS_DATAX1 		0x33	// Read-only
#define GS_DATAY0 		0x34	// Read-only
#define GS_DATAY1 		0x35	// Read-only
#define GS_DATAZ0 		0x36	// Read-only
#define GS_DATAZ1 		0x37

// Define exported functions

// This function must be called at the BEGINNING of the program to enable I2C communication
void I2C0Init(void);

// Use this function for SINGLE BYTE READS.  The return value is the data read from the ADXL
unsigned char ReadGSRegister(unsigned char address);

// Use this function for SINGLE BYTE WRITES.  Specify address and data
void WriteGSRegister(unsigned char address, unsigned char value);

// Use this function for MULTI-BYTE READS.  You NEED this function to properly read sampled accelerations
void MultiReadGS(unsigned char address, unsigned char * values, unsigned char length);

#endif
