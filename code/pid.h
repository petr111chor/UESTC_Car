/*
 * pid.h
 *
 *  Created on: 2023��5��11��
 */

#ifndef PID_H_
#define PID_H_

#include <stdint.h>

typedef struct {
    float kp;         //P
    float ki;         //I
    float kd;         //D
    float imax;       //�����޷�

    float out_p;  //KP���
    float out_i;  //KI���
    float out_d;  //KD���
    float out;    //pid���

    float integrator; //< ����ֵ
    float last_error; //< �ϴ����
    float last_derivative; //< �ϴ���������ϴ����֮��
    unsigned long last_t;     //< �ϴ�ʱ��
} pid_param_t;

typedef struct
{
    float       kp;                         //P
    float       ki;                         //I
    float       kd;                         //D

    float       pid_out_p;                  //Kp���
    float       pid_out_i;                  //Ki���
    float       pid_out_d;                  //Kd���
    float       pid_out_increment;          //����ʽPID���
    float       pid_out;                    //PID���

    float       current_error;              //��ǰƫ��
    float       last_error;                 //��һ�ε�ƫ��
    float       far_error;                  //���ϴε�ƫ��
    float       error_m;                    //�����ֵ

} _pid_param_m;


typedef struct
{
    float               kp;         //С��p
    float               kd;         //С��d
    float               bkp;        //����p
    float               bkd;        //����d
    uint8_t             line;       //������
} _pid_param_s;

void Param_Init(void);
float constrain_float(float amt, float low, float high);
float PidLocCtrl(volatile pid_param_t * pid, float error);
float PidIncCtrl(volatile pid_param_t * pid, int16_t error);

#endif /* PID_H_ */
