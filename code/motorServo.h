/*
 * motor.h
 *
 *  Created on: 2023��5��8��
 *      Author: Kern
 */
#include "zf_common_headfile.h"
#ifndef MOTORSERVO_H_
#define MOTORSERVO_H_

#define Servo_Delta                     (90)                             //�������ת���Ĳ�ֵ�������ͺţ����˺Ͷ���۳��й�
#define Servo_Center_Mid                (730  )                             //���ֱ����ֵ��
#define Servo_Left_Max                  (Servo_Center_Mid+Servo_Delta)      //�����ת����ֵ
#define Servo_Right_Min                 (Servo_Center_Mid-Servo_Delta)      //�����ת����ֵ����ֵ��������÷�ʽ�йأ���ʽ

#define Servo_PWM_PIN                   (ATOM0_CH1_P33_9)// TIM5_PWM_MAP0_CH3_A2

//#define Motor_PWM_Left                  (ATOM0_CH3_P21_5)// TIM8_PWM_MAP0_CH1_C6
//#define Motor_PWM_Right                 (ATOM0_CH1_P21_3)// TIM8_PWM_MAP0_CH2_C7
//#define Motor_GPIO_Left                 (P21_4 )// E4
//#define Motor_GPIO_Right                (P21_2 )// E3


/*                        ���                                                                                   */
void ServoInit                          (unsigned short freq);
void ServoCtrl                          (unsigned short int duty);
/*                        ���                                                                                   */
//void MotorInit                          (unsigned short freq);
//void MotorCtrl                          (short int motor1, short int motor2);


#endif /* MOTORSERVO_H_ */
