#include "address_map_arm.h"
#include "GSInterface.h"

volatile unsigned int * I2C0_CON = (volatile unsigned int*)(I2C0_BASE);
volatile unsigned int * I2C0_TAR = (volatile unsigned int*)(I2C0_BASE + 0x04);
volatile unsigned int * I2C0_FS_SCL_HCNT = (volatile unsigned int*)(I2C0_BASE + 0x1C);
volatile unsigned int * I2C0_FS_SCL_LCNT = (volatile unsigned int*)(I2C0_BASE + 0x20);
volatile unsigned int * I2C0_ENABLE = (volatile unsigned int*)(I2C0_BASE + 0x6C);
volatile unsigned int * I2C0_ENABLE_STATUS = (volatile unsigned int*)(I2C0_BASE + 0x9C);
volatile unsigned int * I2C0_DATA_CMD = (volatile unsigned int*)(I2C0_BASE + 0x10);
volatile unsigned int * I2C0_RXFLR = (volatile unsigned int*)(I2C0_BASE + 0x78);

void I2C0Init(void)
{
	*I2C0_ENABLE = 2;

	while (((*I2C0_ENABLE_STATUS) & 0x1) == 1)
		;

	*I2C0_CON = 0x65;

	*I2C0_TAR = 0x53;

	*I2C0_FS_SCL_HCNT = 60 + 30;
	*I2C0_FS_SCL_LCNT = 130 + 30;

	*I2C0_ENABLE = 1;

	while (((*I2C0_ENABLE_STATUS) & 0x1) == 0)
		;
}

unsigned char ReadGSRegister(unsigned char address)
{
	*I2C0_DATA_CMD = address + 0x400;
	*I2C0_DATA_CMD = 0x100;

	while (*I2C0_RXFLR == 0)
		;
	return *I2C0_DATA_CMD;
}

void WriteGSRegister(unsigned char address, unsigned char value)
{
	*I2C0_DATA_CMD = address + 0x400;
	*I2C0_DATA_CMD = value;
}

void MultiReadGS(unsigned char address, unsigned char * values, unsigned char length)
{
	*I2C0_DATA_CMD = address + 0x400;

	int i;
	for (i = 0; i<length; i++)
		*I2C0_DATA_CMD = 0x100;

	for (i = 0; i<length; i++)
	{
		while (*I2C0_RXFLR == 0)
			;
		values[i] = *I2C0_DATA_CMD;
	}
}
