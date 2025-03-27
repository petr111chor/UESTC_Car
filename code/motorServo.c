#include "zf_common_headfile.h"
//volatile uint16_t CurrentServoDty;
/*************************************************************************
 *  �������ƣ�void ServoInit(void)
 *  ����˵�������PWM��ʼ��
 *  ����˵����freq Ƶ��
 *  �������أ���
 *  �޸�ʱ�䣺
 *  ��    ע��
 *************************************************************************/
void ServoInit(unsigned short freq) {
    pwm_init(Servo_PWM_PIN, freq, Servo_Center_Mid);
}

/*************************************************************************
 *  �������ƣ�void ServoCtrl(uint32_t duty)
 *  ����˵�������ת�Ǻ���������С�����˷�Χ���ƣ���С
 *  ����˵����short duty��ռ�ձ�
 *  �������أ���
 *  �޸�ʱ�䣺
 *  ��    ע��]
 ************************************** ***********************************/
void ServoCtrl(uint16_t duty) {

     if (duty >= Servo_Left_Max)                  //���Ʒ�ֵ
     duty = Servo_Left_Max;
     else if (duty <= Servo_Right_Min)            //���Ʒ�ֵ
     duty = Servo_Right_Min;
     CurrentServoDty = duty;
     pwm_set_duty(Servo_PWM_PIN, duty);
}


/*************************************************************************
 *  �������ƣ�void MotorInit(u16 freq)
 *  ����˵�������PWM��ʼ��
 *  ����˵������
 *  �������أ���
 *  �޸�ʱ�䣺2020��4��1��
 *  ��    ע��PWM��C6 C7   GPIO��E3 E4
 *************************************************************************/
//void MotorInit(unsigned short freq) {
//    gpio_init(Motor_GPIO_Left, GPO, GPIO_HIGH, GPO_PUSH_PULL);
//    gpio_init(Motor_GPIO_Right, GPO, GPIO_HIGH, GPO_PUSH_PULL);

//    pwm_init(Motor_PWM_Left, freq, 0);
//    pwm_init(Motor_PWM_Right, freq, 0);

//}

/*************************************************************************
 *  �������ƣ�void MotorCtrl(int16_t motor1, int16_t motor2)
 *  ����˵��������ٶȿ���
 *  ����˵������
 *  �������أ���
 *  �޸�ʱ�䣺2020��4��1��
 *  ��    ע��PWM��C6 C7   GPIO��E3 E4
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

