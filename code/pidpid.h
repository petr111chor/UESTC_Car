/*
 * pid.h
 *
 *  Created on: 2025Äê3ÔÂ10ÈÕ
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

extern struct PID motor_pid_l;
extern struct PID motor_pid_r;

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

#define pid_out_limit 6000

float PID_Normal(PID*PID ,float now_data ,float target_data);
float PID_Increase(PID*PID ,float now_data ,float target_data);
int limit_int(int a, int b, int c);
#endif /* CODE_PID_H_ */
