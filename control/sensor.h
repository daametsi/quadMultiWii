#ifndef __ITG_SENSOR_HH__
#define __ITG_SENSOR_HH__

class MPU6050;
class Sensor
{
public:
	MPU6050 MPU;
	Sensor();
	~Sensor();
	// return true if init OK
	// init accel, gyro, mag, and baro
	bool Init();

	// calibrate the sensors
	bool Calibrate();

private:

	int file;	// i2c file handle
	bool i2c_bus_init() ;

};

#endif
