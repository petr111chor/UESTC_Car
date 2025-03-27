#include "zf_common_headfile.h"
//volatile uint16_t CurrentServoDty;
/*************************************************************************
 *  函数名称：void ServoInit(void)
 *  功能说明：舵机PWM初始化
 *  参数说明：freq 频率
 *  函数返回：无
 *  修改时间：
 *  备    注：
 *************************************************************************/
void ServoInit(unsigned short freq) {
    pwm_init(Servo_PWM_PIN, freq, Servo_Center_Mid);
}

/*************************************************************************
 *  函数名称：void ServoCtrl(uint32_t duty)
 *  功能说明：舵机转角函数，由于小车拉杆范围限制，较小
 *  参数说明：short duty，占空比
 *  函数返回：无
 *  修改时间：
 *  备    注：]
 ************************************** ***********************************/
void ServoCtrl(uint16_t duty) {

     if (duty >= Servo_Left_Max)                  //限制幅值
     duty = Servo_Left_Max;
     else if (duty <= Servo_Right_Min)            //限制幅值
     duty = Servo_Right_Min;
     CurrentServoDty = duty;
     pwm_set_duty(Servo_PWM_PIN, duty);
}


/*************************************************************************
 *  函数名称：void MotorInit(u16 freq)
 *  功能说明：电机PWM初始化
 *  参数说明：无
 *  函数返回：无
 *  修改时间：2020年4月1日
 *  备    注：PWM：C6 C7   GPIO：E3 E4
 *************************************************************************/
//void MotorInit(unsigned short freq) {
//    gpio_init(Motor_GPIO_Left, GPO, GPIO_HIGH, GPO_PUSH_PULL);
//    gpio_init(Motor_GPIO_Right, GPO, GPIO_HIGH, GPO_PUSH_PULL);

//    pwm_init(Motor_PWM_Left, freq, 0);
//    pwm_init(Motor_PWM_Right, freq, 0);

//}

/*************************************************************************
 *  函数名称：void MotorCtrl(int16_t motor1, int16_t motor2)
 *  功能说明：电机速度控制
 *  参数说明：无
 *  函数返回：无
 *  修改时间：2020年4月1日
 *  备    注：PWM：C6 C7   GPIO：E3 E4
 *************************************************************************/
//void MotorCtrl(int16_t motor1, int16_t motor2)
//{
//    if (motor1 > 0)
//    {
//        pwm_set_duty(Motor_PWM_Left, motor1);
//        gpio_set_level(Motor_GPIO_Left, GPIO_HIGH);
//
//   }
//    else
//    {
//        pwm_set_duty(Motor_PWM_Left, -motor1);
//        gpio_set_level(Motor_GPIO_Left, GPIO_LOW);
//    }

//    if (motor2 > 0)
//    {
//        pwm_set_duty(Motor_PWM_Right, motor2);
//        gpio_set_level(Motor_GPIO_Right, GPIO_HIGH);
//    }
//    else
//    {
//        pwm_set_duty(Motor_PWM_Right, -motor2);
//        gpio_set_level(Motor_GPIO_Right, GPIO_LOW);
//    }
//
//}

