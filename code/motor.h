/*
 * motor.h
 *
 *  Created on: 2025Äê3ÔÂ10ÈÕ
 *      Author: 15208
 */

#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_

#include "zf_common_headfile.h"

#define Motor_PWM_Left                  (ATOM0_CH3_P21_5)// TIM8_PWM_MAP0_CH1_C6
#define Motor_PWM_Right                 (ATOM0_CH1_P21_3)// TIM8_PWM_MAP0_CH2_C7
#define Motor_GPIO_Left                 (P21_2 )// E4
#define Motor_GPIO_Right                (P21_4 )// E3

typedef struct motor1
{
        int target_speed;
        int duty;
        int encoder_speed;
        int encoder_raw;
        int32 total_encoder;
}motor1;

extern struct motor1 motor_l;
extern struct motor1 motor_r;



void MotorInit(unsigned short freq);
void MotorCtrl(int16_t motor1, int16_t motor2);
void Speed_Set(pwm_channel_enum pin1, gpio_pin_enum pin2, int speed ,uint8 plus, uint8 minus);
void Motor_Control(int Speed_L, int Speed_R);
void Motor_ControlR(int Speed_R);
void Motor_ControlL(int Speed_L);
void car_start(void);
int car_turn(uint8 mid);
void Final_Motor_Control(int speed,float k,int mid,int limit);
void Encoder_Init(void);
void Encoder_Data_Get(void);

#endif /* CODE_MOTOR_H_ */
