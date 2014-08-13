// Adxl345.cpp
// Adxl345 accelerator 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <iostream>;
#include "adxl345.h"

using namespace std;


// init Adxl345
bool Adxl345::Init(int _file) {
	file = _file;
	
	bool r = false;

	// adxl init code
	char write_buf_1[2] = { 0x2D, 0x08 };	//  register: Power CTRL  -- value: Set measure bit 3 on
	char write_buf_2[2] = { 0x31, 0x0B };	//  register: DATA_FORMAT -- value: Set bits 3(full range) and 1 0 on (+/- 16g-range)
	char write_buf_3[3] = { 0x2C, 0x09 }; 	//  register: BW_RATE     -- value: rate=50hz, bw=20hz

	int adapter_nr = 3; 
	int i2c_addr = 0x53; // itg3200 I2C address 

	
	if (ioctl(file, I2C_SLAVE, i2c_addr) < 0) {
		printf("Failed to acquire bus access and/or talk to slave.\n");
		/* ERROR HANDLING; you can check errno to see what went wrong */
		if (file >= 0)
			close(file);
		return false;
	} else {
		printf("Comm to adxl is open.\n");
	}


	if (write(file, write_buf_1, 2) != 2) /* first init string */
	{
		printf("i2c send %x failed\n", i2c_addr);
		close(file);
		exit(1);

	} else {
		printf("i2c send buf[1] success.\n");
	}
	sleep(0.1);
	if (write(file, write_buf_2, 2) != 2) /* second init string */
	{
		printf("i2c send %x failed\n", i2c_addr);
		close(file);
		exit(1);
	} else {
		sleep(0.1);
		printf("i2c send buf[2] success.\n");
	}

	if (write(file, write_buf_3, 2) != 2) /* second init string */
	{
		printf("i2c send %x failed\n", i2c_addr);
		close(file);
		exit(1);
	} else {
		sleep(0.1);	
		printf("i2c send buf[3] success.\n");
	}

	return true;
}


Adxl345::Adxl345()
{
	x = 0;
	y = 0;
	z = 0;
}
Adxl345::~Adxl345()
{

}
// return true if Calibrate OK
bool Adxl345::Calibrate()
{

	cout << "ADXL 345 Calibrate" << endl;
	int i2c_addr = 0x53; // itg3200 I2C address 

	m_xOffset = 0;
	m_yOffset = 0;
	m_zOffset = 0;
	int count = 400;
	short x1, y1, z1;
	unsigned char buf2[8] = {0,0,0,0,0,0,0,0};
	int rb = 8;
	unsigned char buf1[1] = {0x80|0x30};
	// 0x30 is status byte, 
	// 0x80| 0x 30, set auto increasement of register, so it will read 7 bytes in one commands
	if (ioctl(file, I2C_SLAVE, i2c_addr) < 0) {
		printf("Failed to acquire bus access and/or talk to slave.\n");
		/* ERROR HANDLING; you can check errno to see what went wrong */
		return false;
	} 	
	for(int i = 0; i < count; i ++)
	{
		if (write(file, buf1, 1) != 1) /* send to format data */
		{
			printf("I2C Send %x Failed\n", i2c_addr);
			close(file);
			exit(1);
		}

		// delay for adjusting reporting speed. Bad data without.
		if (read(file, buf2, rb) != rb) 
		{
			printf("I2C Send %x Failed\n", i2c_addr);
		}
		if((buf2[0] & 0x80 )== 0x80)	// new set of data is availabe, bit 7 is flag for new data ready
		{
			unsigned char x2[2] = {buf2[2], buf2[3]};
			memcpy(&x1, x2, 2);
			unsigned char y2[2] = {buf2[4], buf2[5]};
			memcpy(&y1, y2, 2);
			unsigned char z2[2] = {buf2[6], buf2[7]};
			memcpy(&z1, z2, 2);
			m_xOffset += x1;
			m_yOffset += y1;
			m_zOffset += z1;
		}
		else
		{
			usleep(10000);	
			i--;	// read again
			printf(" Wrong data %d\n", buf2[0]);
		}
		usleep(25000);
	
	}
		
	m_xOffset = m_xOffset/count;
	m_yOffset = m_yOffset/count;
	m_zOffset = m_zOffset/count - 265;	// 265 is adxl calibrate data 
	cout << "X offset: " << m_xOffset << " Y offset:" << m_yOffset << " Z offset:" << m_zOffset << endl;
	return true;

}
// return true if data been updated correctly.
bool Adxl345::UpdateData()
{
	int i2c_addr = 0x53; // itg3200 I2C address 
	
	if (ioctl(file, I2C_SLAVE, i2c_addr) < 0) {
		printf("Failed to acquire bus access and/or talk to slave.\n");
		/* ERROR HANDLING; you can check errno to see what went wrong */
		return false;
	} 
	unsigned char buf2[8] = {0,0,0,0,0,0,0,0};
	int rb = 8;
	unsigned char buf1[1] = {0x30 | 0x80};	// read 8 bytes from 0x30, the x, y, z data start from third bytes, the first bytes is status byte 
	if (write(file, buf1, 1) != 1) /* send to format data */
	{
		printf("I2C Send %x Failed\n", i2c_addr);
		close(file);
		exit(1);
	}
	//sleep(0.001);
	// delay for adjusting reporting speed. Bad data without.
	if (read(file, buf2, rb) != rb) 
	{
		printf("I2C Send %x Failed\n", i2c_addr);
	}
	if((buf2[0] & 0x80) == 0x80)		// if the data is ready
	{
		short x1, y1, z1;
		unsigned char x2[2] = {buf2[2], buf2[3]};
		memcpy(&x1, x2, 2);
		unsigned char y2[2] = {buf2[4], buf2[5]};
		memcpy(&y1, y2, 2);
		unsigned char z2[2] = {buf2[6], buf2[7]};
		memcpy(&z1, z2, 2);
		
		x1 = x1 - m_xOffset;
		y1 = y1 - m_yOffset;
		z1 = z1 - m_zOffset;
		
		x = x1 ;
		y = y1 ;
		z = -z1 ;

		return true;
	}
	else
	{
		//printf(" Wrong data %d\n", buf2[0]);
	}
	return false;

}

