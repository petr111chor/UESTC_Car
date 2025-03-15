/*
 * imu.h
 *
 *  Created on: 2023��7��1��
 *      Author: Kern
 */

#ifndef IMU_H_
#define IMU_H_



typedef struct {
    double pitch;    //������
    double roll;     //�����
    double yaw;      //�����
} euler_param_t;   //ŷ����

struct angle_t
{
    float x;
    float y;
    float z;
};


extern euler_param_t eulerAngle;              // ŷ����
extern struct angle_t  GyroOffset;

void get_yaw(void);
void get_pitch(void);

#endif /* IMU_H_ */
