// Sensor.cpp
// demo of read 10 dof sendor board

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <iostream>;
#include "itg4200.h"
#include "adxl345.h"
#include "sensor.h"

using namespace std;



/* I2C busses, u-boot are 0 - 2, userspace is 1 - 3 */

/* i2c_bus_init() open i2c bus 3 if available */
bool Sensor::i2c_bus_init() {
	bool r = false;
	char filename[20];
	sprintf(filename, "/dev/i2c-1");
	if ((file = open(filename, O_RDWR)) < 0) {
		printf("Failed to open the i2c bus .\n");
		/* ERROR HANDLING; you can check errno to see what went wrong */
		if (file >= 0)
			close(file);
		r = false;
	} else 
	{
		printf("Opened /dev/i2c-1\n");
		r = true;
	}
	return r;
}

// init itg3200
bool Sensor::Init() {
	if(i2c_bus_init())
	{
		Gyro.Init(file);
		Acc.Init(file);
	}
	
	return true;
}


Sensor::Sensor()
{
}
Sensor::~Sensor()
{

}

// return true if init OK
bool Sensor::Calibrate()
{
	cout << "Calibrate sensors" << endl;
	Gyro.Calibrate();
	//Acc.Calibrate();
	return true;

}

