

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <iostream>;
#include "mpu6050.h"

using namespace std;


// mpu6050 init codes
char write_buf_1[2] = { 0x6B, 0x21 };// REG 6B: PWR_MGMT_1: device reset off, sleep off, cycle on, temp sensor on, clksel PLL with xaxis gyro ref
char write_buf_2[2] = { 0x6C, 0x80 };// REG 6C: PWR_MGMT_2: 20Hz wakeup low power mode, no accel/gyro standby 

// mpu6050 ac init code
char write_buf_3[3] = { 0x1C, 0x18 };// REG 1C: ACCEL_CONFIG: no selftest, +/-16g

char GYRO_XOUT_L[1] = { 0x44 };
char GYRO_XOUT_H[1] = { 0x43 };
char GYRO_YOUT_L[1] = { 0x46 };
char GYRO_YOUT_H[1] = { 0x45 };
char GYRO_ZOUT_L[1] = { 0x48 };
char GYRO_ZOUT_H[1] = { 0x47 };
char INT_STATUS[1] = {0x3A };

char buf[8];

int adapter_nr = 3;
int i2c_addr = 0x68;

int i;

int read_bytes = 1;



// init mpu6050
bool MPU6050::Init(int _file) {
	file = _file;

	bool r = false;
	i2c_addr = 0x68;

	if (ioctl(file, I2C_SLAVE, i2c_addr) < 0) {
		printf("Failed to acquire bus access and/or talk to slave.\n");
		/* ERROR HANDLING; you can check errno to see what went wrong */
		if (file >= 0)
			close(file);
		return false;
	} else {
		printf("Comm to mpu6050 is open.\n");
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


MPU6050::MPU6050()
{
	x = 0;
	y = 0;
	z = 0;
	xAC = 0;
	yAC = 0;
	zAC = 0;
}
MPU6050::~MPU6050()
{

}
// return true if Calibrate OK
bool MPU6050::Calibrate()
{

	m_xOffset = 0;
	m_yOffset = 0;
	m_zOffset = 0;
	m_xOffsetAC = 0;
	m_yOffsetAC = 0;
	m_zOffsetAC = 0;
	int count = 400;
	short x1, y1, z1;
	unsigned char buf2[7] = {0,0,0,0,0,0,0};
	int rb = 7;
//	unsigned char buf1[6] = {0x28,0x29,0x2A,0x2B,0x2C,0x2D};
	unsigned char buf1[1] = {0xA7};//,0x29,0x2A,0x2B,0x2C,0x2D};(update!!!)
	// 0x27 is status byte,(update!!)
	// 0x80| 0x 27, set auto increasement of register, so it will read 7 bytes in one commands
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
		if((buf2[0] & 0x8 )== 0x8)	// new set of data is available
		{
			unsigned char x2[2] = {buf2[1], buf2[2]};
			memcpy(&x1, x2, 2);
			unsigned char y2[2] = {buf2[3], buf2[4]};
			memcpy(&y1, y2, 2);
			unsigned char z2[2] = {buf2[5], buf2[6]};
			memcpy(&z1, z2, 2);
			m_xOffset += x1;
			m_yOffset += y1;
			m_zOffset += z1;
		}
		else
		{
			usleep(10000);
			i--;	// read again
			//printf(" Wrong data %d\n", buf2[0]);
		}
		usleep(2500);

	}

	m_xOffset = m_xOffset/count;
	m_yOffset = m_yOffset/count;
	m_zOffset = m_zOffset/count;
	cout << "X offset: " << m_xOffset << " Y offset:" << m_yOffset << " Z offset:" << m_zOffset << endl;
	return true;

}
// return true if data been updated correctly.
bool MPU6050::UpdateData()
{

	if (ioctl(file, I2C_SLAVE, i2c_addr) < 0) {
		printf("Failed to acquire bus access and/or talk to slave.\n");
		/* ERROR HANDLING; you can check errno to see what went wrong */
		return false;
	}
	unsigned char buf2[6] = {0,0,0,0,0,0};
	int rb = 6;
	unsigned char buf1[1] = {0x43};//,0x29,0x2A,0x2B,0x2C,0x2D};
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
	if((buf2[0] & 0x8) == 0x8)
	{
		short x1, y1, z1;
		unsigned char x2[2] = {buf2[1], buf2[2]};
		memcpy(&x1, x2, 2);
		unsigned char y2[2] = {buf2[3], buf2[4]};
		memcpy(&y1, y2, 2);
		unsigned char z2[2] = {buf2[5], buf2[6]};
		memcpy(&z1, z2, 2);

		x1 = x1 - m_xOffset;
		y1 = y1 - m_yOffset;
		z1 = z1 - m_zOffset;

		x = x1 / 30;
		y = y1 / 30;
		z = -z1 /30;

		return true;
	}
	else
	{
		//printf(" Wrong data %d\n", buf2[0]);
	}
	return false;

}
// return true if data been updated correctly.
bool MPU6050::UpdateAcc()
{

	if (ioctl(file, I2C_SLAVE, i2c_addr) < 0) {
		printf("Failed to acquire bus access and/or talk to slave.\n");
		/* ERROR HANDLING; you can check errno to see what went wrong */
		return false;
	}
	unsigned char buf2[6] = {0,0,0,0,0,0};
	int rb = 6;
	unsigned char buf1[1] = {0x3B};//,0x29,0x2A,0x2B,0x2C,0x2D};
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
	if((buf2[0] & 0x8) == 0x8)
	{
		short x1, y1, z1;
		unsigned char x2[2] = {buf2[1], buf2[2]};
		memcpy(&x1, x2, 2);
		unsigned char y2[2] = {buf2[3], buf2[4]};
		memcpy(&y1, y2, 2);
		unsigned char z2[2] = {buf2[5], buf2[6]};
		memcpy(&z1, z2, 2);

		x1 = x1 - m_xOffset;
		y1 = y1 - m_yOffset;
		z1 = z1 - m_zOffset;

		xAC = x1 / 30;
		yAC = y1 / 30;
		zAC = -z1 /30;

		return true;
	}
	else
	{
		//printf(" Wrong data %d\n", buf2[0]);
	}
	return false;

}

