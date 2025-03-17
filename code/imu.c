/*
 * imu.c
 *
 *  Created on: 2023年7月1日
 *      Author: Kern
 */

#include "zf_common_headfile.h"


euler_param_t eulerAngle;              // 欧拉角
struct angle_t  Gyro_IMU;
struct angle_t  GyroOffset;


void get_yaw(void)
{
    imu963ra_get_gyro();
//    imu963ra_gyro_z -= - GyroOffset.Zdata;
    Gyro_IMU.z=((float)imu963ra_gyro_z + GyroOffset.z)/14.3;
//    imu963ra_gyro_z = imu963ra_gyro_z+0.061035;//进行角速度零飘补偿
    if((Gyro_IMU.z>=0.1||Gyro_IMU.z<=-0.1)&&(ABS(Gyro_IMU.z)<1000))
    {
        eulerAngle.yaw += -Gyro_IMU.z*0.005;
    }

}


void get_pitch(void)
{
    imu963ra_get_gyro();
//    imu963ra_gyro_z -= - GyroOffset.Zdata;
    Gyro_IMU.x=((float)imu963ra_gyro_x - GyroOffset.x)/14.3;
//    imu963ra_gyro_z = imu963ra_gyro_z+0.061035;//进行角速度零飘补偿
    if((Gyro_IMU.x>=0.1||Gyro_IMU.x<=-0.1)&&(ABS(Gyro_IMU.x)<1000))
    {
        eulerAngle.pitch += -Gyro_IMU.x*0.005;
    }

}
