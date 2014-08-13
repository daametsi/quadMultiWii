#ifndef __ITG_4200_HH__
#define __ITG_4200_HH__

class ITG4200
{
public:
long x, y, z;	// value of x, y, z, valid after update
ITG4200();
~ITG4200();
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