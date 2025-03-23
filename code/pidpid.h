/*
 * pid.h
 *
 *  Created on: 2025年3月10日
 *      Author: 15208
 */

#ifndef CODE_PID_H_
#define CODE_PID_H_

#include "zf_common_headfile.h"

typedef struct PID
{
        float Kp;
        float Ki;
        float Kd;
        float LowPass;

        float Out_P;
        float Out_I;
        float Out_D;

        float PrevError;
        float LastError;
        float Error;
        float Lastdata;
}PID;


typedef struct pid_param_tt{
    float kp;         //P
    float ki;         //I
    float kd;         //D
    float imax;       //积分限幅

    float out_p;  //KP输出
    float out_i;  //KI输出
    float out_d;  //KD输出
    float out;    //pid输出

    float integrator; //< 积分值
    float last_error; //< 上次误差
    float last_derivative; //< 上次误差与上上次误差之差
    unsigned long last_t;     //< 上次时间
} pid_param_tt;


extern struct PID motor_pid_l;
extern struct PID motor_pid_r;

extern struct pid_param_tt motor_pid_left;
extern struct pid_param_tt motor_pid_right;
extern struct pid_param_tt motor_pid_direction;

#define PID_CREATE(_kp,_ki,_kd,_low_pass)\
{                                       \
    .Kp=_kp,                            \
    .Ki=_ki,                            \
    .Kd=_kd,                            \
    .LowPass=_low_pass,                 \
    .Out_P=0,                           \
    .Out_I=0,                           \
    .Out_D=0,                           \
}

#define PID_CREATETEST(_kp,_ki,_kd)\
{                                       \
    .kp=_kp,                            \
    .ki=_ki,                            \
    .kd=_kd,                            \
    .out_p=0,                           \
    .out_i=0,                           \
    .out_d=0,                           \
}


#define pid_out_limit 5000

float PID_Normal(PID*PID ,float now_data ,float target_data);
float PID_Increase(PID*PID ,float now_data ,float target_data);
float PidIncCtrltest(volatile pid_param_tt * pid, int16_t error);
float PidLocCtrltest(volatile pid_param_tt * pid, float error);
int limit_int(int a, int b, int c);
#endif /* CODE_PID_H_ */
