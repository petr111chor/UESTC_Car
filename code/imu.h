/*
 * imu.h
 *
 *  Created on: 2023Äê7ÔÂ1ÈÕ
 *      Author: Kern
 */

#ifndef IMU_H_
#define IMU_H_



typedef struct {
    double pitch;    //¸©Ñö½Ç
    double roll;     //ºá¹ö½Ç
    double yaw;      //º½Ïò½Ç
} euler_param_t;   //Å·À­½Ç

struct angle_t
{
    float x;
    float y;
    float z;
};


extern euler_param_t eulerAngle;              // Å·À­½Ç
extern struct angle_t  GyroOffset;

void get_yaw(void);
void get_pitch(void);

#endif /* IMU_H_ */
