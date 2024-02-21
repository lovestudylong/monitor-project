#ifndef MPU_6050_H
#define MPU_6050_H

struct accel_data {
	short x;
	short y;
	short z;
};
struct gyro_data {
	short x;
	short y;
	short z;
};

union mpu6050_data {
	struct accel_data accel;
	struct gyro_data gyro;
	short temp;
};

#define MPU6050_MAGIC 'K'

#define GET_ACCEL _IOR(MPU6050_MAGIC, 0, union mpu6050_data)
#define GET_GYRO  _IOR(MPU6050_MAGIC, 1, union mpu6050_data)
#define GET_TEMP  _IOR(MPU6050_MAGIC, 2, union mpu6050_data)


#endif
