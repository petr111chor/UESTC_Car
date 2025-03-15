/*
 * pid.c
 *
 *  Created on: 2023��5��11��
 */



#include "zf_common_headfile.h"



/*************************************************************************
 *  �������ƣ�float constrain_float(float amt, float low, float high)
 *  ����˵����pid����ʽ���������
 *  ����˵����
 * @param    pid     pid����
 * @param    error   pid�������
 *  �������أ�PID������   ע���������Ѿ��������ϴν��
 *  �޸�ʱ�䣺2020��4��1��
 *  ��    ע��
 *************************************************************************/
float PidIncCtrl(volatile pid_param_t * pid, int16_t error) {

    pid->out_p = pid->kp * (error - pid->last_error);
    pid->out_i = pid->ki * error;
    pid->out_d = pid->kd * ((error - pid->last_error) - pid->last_derivative);

    pid->last_derivative = error - pid->last_error;
    pid->last_error = error;

    pid->out += pid->out_p + pid->out_i + pid->out_d;

    pid->out = LIMIT(pid->out, -10000, 10000);

    return pid->out;
}


/*************************************************************************
 *  �������ƣ�float constrain_float(float amt, float low, float high)
 *  ����˵����pidλ��ʽ���������
 *  ����˵����
 * @param    pid     pid����
 * @param    error   pid�������
 *  �������أ�PID������
 *  �޸�ʱ�䣺2020��4��1��
 *  ��    ע��
 *************************************************************************/
float PidLocCtrl(volatile pid_param_t * pid, float error) {
    /* �ۻ���� */
    pid->integrator += error;

    /* ����޷� */
    pid->integrator = constrain_float(pid->integrator, -pid->imax, pid->imax);

    pid->out_p = pid->kp * error;
    pid->out_i = pid->ki * pid->integrator;
    pid->out_d = pid->kd * (error - pid->last_error);

    pid->last_error = error;

    pid->out = pid->out_p + pid->out_i + pid->out_d;

    return pid->out;
}



/*PID��ʼ��*/
void Param_Init(void){
    ui_load_to_data();

    if(Garage_Out == 0)
    {
        Flag.Garage_Out = false;
    }

    uint8_t i = 0;
    for (i = 0; i < 10; i++)
    {
        image.cam_finalCenterERR[i] = 0;
    }

    //��� PD����
    Servo_Adc_pid.kp = servo_param.Ui_Adc_SP;
    Servo_Adc_pid.kd = servo_param.Ui_Adc_SD;
    Servo_Cam_pid.kp = servo_param.Ui_Cam_SP;
    Servo_Cam_pid.kd = servo_param.Ui_Cam_SD;

    //��� PID����
    Motor_Left_pid.kp = motor_param.Ui_Motor_P;
    Motor_Left_pid.ki = motor_param.Ui_Motor_I;
    Motor_Left_pid.kd = 0;
    Motor_Left_pid.imax = 0;
    Motor_Left_pid.out_p = 0;
    Motor_Left_pid.out_i = 0;
    Motor_Left_pid.out_d = 0;
    Motor_Left_pid.out = 0;
    Motor_Left_pid.integrator = 0;
    Motor_Left_pid.last_error = 0;
    Motor_Left_pid.last_derivative = 0;
    Motor_Left_pid.last_t = 0;

    Motor_Rigt_pid.kp = motor_param.Ui_Motor_P;
    Motor_Rigt_pid.ki = motor_param.Ui_Motor_I;
    Motor_Rigt_pid.kd = 0;
    Motor_Rigt_pid.imax = 0;
    Motor_Rigt_pid.out_p = 0;
    Motor_Rigt_pid.out_i = 0;
    Motor_Rigt_pid.out_d = 0;
    Motor_Rigt_pid.out = 0;
    Motor_Rigt_pid.integrator = 0;
    Motor_Rigt_pid.last_error = 0;
    Motor_Rigt_pid.last_derivative = 0;
    Motor_Rigt_pid.last_t = 0;

    Motor_Dir_pid.kp = motor_param.Ui_Dir_P;
    Motor_Dir_pid.ki = motor_param.Ui_Dir_I;
    Motor_Dir_pid.kd = motor_param.Ui_Dir_D;
    Motor_Dir_pid.imax = motor_param.Ui_Dir_INT_Thr;
    Motor_Dir_pid.out_p = 0;
    Motor_Dir_pid.out_i = 0;
    Motor_Dir_pid.out_d = 0;
    Motor_Dir_pid.out = 0;
    Motor_Dir_pid.integrator = 0;
    Motor_Dir_pid.last_error = 0;
    Motor_Dir_pid.last_derivative = 0;
    Motor_Dir_pid.last_t = 0;

    Motor_Dir_A_pid.kp = motor_param.Ui_Dir_A_P;
    Motor_Dir_A_pid.ki = motor_param.Ui_Dir_A_I;
    Motor_Dir_A_pid.kd = motor_param.Ui_Dir_A_D;
    Motor_Dir_A_pid.imax = motor_param.Ui_Dir_INT_Thr;
    Motor_Dir_A_pid.out_p = 0;
    Motor_Dir_A_pid.out_i = 0;
    Motor_Dir_A_pid.out_d = 0;
    Motor_Dir_A_pid.out = 0;
    Motor_Dir_A_pid.integrator = 0;
    Motor_Dir_A_pid.last_error = 0;
    Motor_Dir_A_pid.last_derivative = 0;
    Motor_Dir_A_pid.last_t = 0;


    ctrl_dis_pid.kp = dis_param.ctrl_dis_P;
    ctrl_dis_pid.ki = dis_param.ctrl_dis_I;
    ctrl_dis_pid.kd = dis_param.ctrl_dis_D;

    ctrl_dis_pid.imax = 0;
    ctrl_dis_pid.out_p = 0;
    ctrl_dis_pid.out_i = 0;
    ctrl_dis_pid.out_d = 0;
    ctrl_dis_pid.out = 0;
    ctrl_dis_pid.integrator = 0;
    ctrl_dis_pid.last_error = 0;
    ctrl_dis_pid.last_derivative = 0;
    ctrl_dis_pid.last_t = 0;
}

