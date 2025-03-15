/*
 * pid.h
 *
 *  Created on: 2023年5月11日
 */

#ifndef PID_H_
#define PID_H_

#include <stdint.h>

typedef struct {
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
} pid_param_t;

typedef struct
{
    float       kp;                         //P
    float       ki;                         //I
    float       kd;                         //D

    float       pid_out_p;                  //Kp输出
    float       pid_out_i;                  //Ki输出
    float       pid_out_d;                  //Kd输出
    float       pid_out_increment;          //增量式PID输出
    float       pid_out;                    //PID输出

    float       current_error;              //当前偏差
    float       last_error;                 //上一次的偏差
    float       far_error;                  //上上次的偏差
    float       error_m;                    //误差阈值

} _pid_param_m;


typedef struct
{
    float               kp;         //小弯p
    float               kd;         //小弯d
    float               bkp;        //大弯p
    float               bkd;        //大弯d
    uint8_t             line;       //控制行
} _pid_param_s;

void Param_Init(void);
float constrain_float(float amt, float low, float high);
float PidLocCtrl(volatile pid_param_t * pid, float error);
float PidIncCtrl(volatile pid_param_t * pid, int16_t error);

#endif /* PID_H_ */
