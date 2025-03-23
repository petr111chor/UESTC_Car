/*
 * motor.c
 *
 *  Created on: 2025年3月10日
 *      Author: 15208
 */
#include "zf_common_headfile.h"

motor1 motor_l;
motor1 motor_r;
PID motor_dir  =PID_CREATE(1,0,0,1);
PID motor_pid_l=PID_CREATE(20,0.2,0,1);//电机
PID motor_pid_r=PID_CREATE(20,0.2,0,1);//电机
pid_param_tt motor_pid_left=PID_CREATETEST(2,0.05,0);
pid_param_tt motor_pid_right=PID_CREATETEST(2,0.05,0);
pid_param_tt motor_pid_direction=PID_CREATETEST(1,0.1,0.6);//转向

volatile extern uint32_t start = 0;

//电机初始化
void MotorInit(unsigned short freq)
{
    gpio_init(Motor_GPIO_Left, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(Motor_GPIO_Right, GPO, GPIO_HIGH, GPO_PUSH_PULL);

    pwm_init(Motor_PWM_Left, freq, 0);
    pwm_init(Motor_PWM_Right, freq, 0);

}


//电机控制
void MotorCtrl(int16_t motor1, int16_t motor2)
{
    if (motor1 > 0.0)
    {
        pwm_set_duty(Motor_PWM_Left, motor1);
        gpio_set_level(Motor_GPIO_Left, GPIO_HIGH);

    }
    else
    {
        pwm_set_duty(Motor_PWM_Left, -motor1);
        gpio_set_level(Motor_GPIO_Left, GPIO_LOW);
    }

    if (motor2 > 0)
    {
        pwm_set_duty(Motor_PWM_Right, motor2);
        gpio_set_level(Motor_GPIO_Right, GPIO_HIGH);
    }
    else
    {
        pwm_set_duty(Motor_PWM_Right, -motor2);
        gpio_set_level(Motor_GPIO_Right, GPIO_LOW);
    }

}

//设置单独电机
void Speed_Set(pwm_channel_enum pin1, gpio_pin_enum pin2, int speed ,uint8 plus, uint8 minus)
{
    if(speed>0)
    {
        pwm_set_duty(pin1, speed);
        gpio_set_level(pin2, plus);
    }
    else
    {
        pwm_set_duty(pin1, -speed);
        gpio_set_level(pin2, minus);
    }
}


//电机速度闭环控制
void Motor_Control(int Speed_L, int Speed_R)
{
    motor_l.target_speed=Speed_L;
    motor_r.target_speed=Speed_R;

    motor_l.duty=limit_int(0,motor_l.duty+PID_Increase(&motor_pid_l,(float)motor_l.encoder_speed,(float)motor_l.target_speed),pid_out_limit);
    motor_r.duty=limit_int(0,motor_r.duty+PID_Increase(&motor_pid_r,(float)motor_r.encoder_speed,(float)motor_r.target_speed),pid_out_limit);

    Speed_Set(Motor_PWM_Left,Motor_GPIO_Left,motor_l.duty,0,1);
    Speed_Set(Motor_PWM_Right,Motor_GPIO_Right,motor_r.duty,0,1);
}


void Motor_ControlR(int Speed_R)
{
    //motor_l.target_speed=Speed_L;
    motor_r.target_speed=Speed_R;

    //motor_l.duty=limit_int(-pid_out_limit,motor_l.duty+PID_Increase(&motor_pid_l,(float)motor_l.encoder_speed,(float)motor_l.target_speed),pid_out_limit);
    motor_r.duty=limit_int(-pid_out_limit,motor_r.duty+PID_Increase(&motor_pid_r,(float)motor_r.encoder_speed,(float)motor_r.target_speed),pid_out_limit);

    //Speed_Set(Motor_PWM_Left,Motor_GPIO_Left,motor_l.duty,0,1);
    Speed_Set(Motor_PWM_Right,Motor_GPIO_Right,motor_r.duty,0,1);
}

void Motor_ControlL(int Speed_L)
{
    //motor_l.target_speed=Speed_L;
    motor_l.target_speed=Speed_L;

    //motor_l.duty=limit_int(-pid_out_limit,motor_l.duty+PID_Increase(&motor_pid_l,(float)motor_l.encoder_speed,(float)motor_l.target_speed),pid_out_limit);
    motor_l.duty=limit_int(-pid_out_limit,motor_l.duty+PID_Increase(&motor_pid_l,(float)motor_l.encoder_speed,(float)motor_l.target_speed),pid_out_limit);

    //Speed_Set(Motor_PWM_Left,Motor_GPIO_Left,motor_l.duty,0,1);
    Speed_Set(Motor_PWM_Left,Motor_GPIO_Left,motor_l.duty,0,1);
}



//后续要把这个函数放到中断里调用，但是标志位计数怎么办？？
void car_start(void)
{

    if(key_get_state(KEY_1)==KEY_SHORT_PRESS)
    {
        start++;
    }
    if(start!=0)
    {
        if(start%2==1)
        {
            Motor_Control(80, 80);
        }
        else
        {
            Motor_Control(0, 0);
        }
    }
}
extern int ssss=10;

void car_turn(float mid)
{
    int actuall_speed_difference;
    int turn_value;
    int left_target_speed,right_target_speed;
    actuall_speed_difference=motor_r.encoder_speed-motor_l.encoder_speed;//不知道这个值能不能取负值
    turn_value=PID_Normal(&motor_dir,(float)actuall_speed_difference,mid);
    //ssss=turn_value;
    left_target_speed=motor_l.encoder_speed-turn_value;
    right_target_speed=motor_r.encoder_speed+turn_value;
    Motor_Control(left_target_speed, right_target_speed);
    //ips200_show_string(10,240,"turn_value"); ips200_show_int(110,240,turn_value,3);
    //return turn_value;
}


void Final_Motor_Control(int speed,float k,float mid,int limit)
{
    float difference_speed;
    difference_speed = PidLocCtrltest(&motor_pid_direction,mid);
    ssss=difference_speed;
    Motor_Control(limit_int(speed-limit,speed+k*difference_speed,speed+limit),limit_int(speed-limit,speed-k*difference_speed,speed+limit));

}


























void Encoder_Init(void)
{
    //右编码器
    encoder_dir_init(TIM6_ENCODER,TIM6_ENCODER_CH1_P20_3,TIM6_ENCODER_CH2_P20_0);
    //左编码器
    encoder_dir_init(TIM4_ENCODER,TIM4_ENCODER_CH1_P02_8,TIM4_ENCODER_CH2_P00_9);
}


void Encoder_Data_Get(void)
{
  motor_r.encoder_raw=encoder_get_count(TIM6_ENCODER);
  motor_r.encoder_speed=motor_r.encoder_speed*0.2+motor_r.encoder_raw*0.8;
  motor_r.total_encoder+=motor_r.encoder_raw;
  encoder_clear_count(TIM6_ENCODER);

  motor_l.encoder_raw=-encoder_get_count(TIM4_ENCODER);
  motor_l.encoder_speed=motor_l.encoder_speed*0.2+motor_l.encoder_raw*0.8;
  motor_l.total_encoder+=motor_l.encoder_raw;
  encoder_clear_count(TIM4_ENCODER);
}



