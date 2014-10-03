#ifndef __MPU_6050_HH__
#define __MPU_6050_HH__

class MPU6050
{
public:
long x, y, z, xAC, yAC, zAC;	// value of x, y, z, valid after update
MPU6050();
~MPU6050();
// return true if init OK
bool Init(int file);
// return true if data been updated correctly.
bool UpdateData();
// return true if data been updated correctly.
bool UpdateAcc();
// Calibrate the gyro
bool Calibrate();
// Calibrate the accel
bool CalibrateAcc();
private:
long m_xOffset, m_yOffset, m_zOffset, m_xOffsetAC, m_yOffsetAC, m_zOffsetAC;

private :
	int file;	// i2c file handle

};

#endif
