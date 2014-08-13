#ifndef __ADXL345_HH__
#define __ADXL345_HH__

class Adxl345
{
public:
	long x, y, z;	// value of x, y, z, valid after update
	Adxl345();
	~Adxl345();
	// return true if init OK
	bool Init(int file);
	// return true if data been updated correctly.
	bool UpdateData();
	// Calibrate the gyro
	bool Calibrate();
	private:
	long m_xOffset, m_yOffset, m_zOffset;

private :
	int file;	// i2c file handle

};

#endif