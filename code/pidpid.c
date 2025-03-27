/*
 * pid.c
 *
 *  Created on: 2025年3月10日
 *      Author: 15208
 */

#include "zf_common_headfile.h"

//位置式PID  实际值 目标值       图像那边是传过来目标中线值
float PID_Normal(PID*PID ,float now_data ,float target_data)
{
    PID->Error=target_data-now_data;
    PID->Out_D=(PID->Error-PID->Out_P)*PID->LowPass+PID->Out_D*(1-PID->LowPass);
    PID->Out_P=PID->Error;
    return (PID->Kp * PID->Out_P +PID->Kd * PID->Out_D);

}


//增量式PID
float PID_Increase(PID*PID ,float now_data ,float target_data)
{
    PID->Error=target_data-now_data;
    PID->Out_P=(PID->Error - PID->LastError);
    PID->Out_I=PID->Error;
    PID->Out_D=(PID->Error -2*PID->LastError + PID->PrevError);
    PID->PrevError=0.9 * PID->LastError + 0.1 * PID->PrevError;
    PID->LastError=0.9 *PID->Error + 0.1 * PID->LastError;
    PID->Lastdata = now_data;

    return (PID->Kp*PID->Out_P+PID->Ki*PID->Out_I+PID->Kd*PID->Out_D);
}



float PidIncCtrltest(volatile pid_param_tt * pid, int16_t error) {

    pid->out_p = pid->kp * (error - pid->last_error);
    pid->out_i = pid->ki * error;
    pid->out_d = pid->kd * ((error - pid->last_error) - pid->last_derivative);

    pid->last_derivative = error - pid->last_error;
    pid->last_error = error;

    pid->out += pid->out_p + pid->out_i + pid->out_d;

    pid->out = LIMIT(pid->out, -10000, 10000);

    return pid->out;
}


float PidLocCtrltest(volatile pid_param_tt * pid, float error) {
    /* 累积误差 */
    pid->integrator += error;

    /* 误差限幅 */
    pid->integrator = constrain_float(pid->integrator, -pid->imax, pid->imax);

    pid->out_p = pid->kp * error;
    pid->out_i = pid->ki * pid->integrator;
    pid->out_d = pid->kd * (error - pid->last_error);

    pid->last_error = error;

    pid->out = pid->out_p + pid->out_i + pid->out_d;

    return pid->out;
}


int limit_int(int a, int b, int c)
{
    // 自动确定最小最大值，避免参数顺序错误
    int real_min = a < c ? a : c;
    int real_max = a < c ? c : a;

    return (b < real_min) ? real_min :
           (b > real_max) ? real_max : b;
}


