    /*
 * variables.c
 *
 *  Created on: 2023��5��11��
 *      Author: ZGX
 */
#include "zf_common_headfile.h"
#include "motorServo.h"
/************************************������******************************************/
volatile short Cam_Process_Result;

uint8_t send_data_flag = 0;//��־λ����

/************************************PID�ṹ��***********************************************/
volatile pid_param_t  Motor_Left_pid;         //��������PID
volatile pid_param_t  Motor_Rigt_pid;          //�ҵ������PID
volatile pid_param_t  Motor_Dir_pid;
volatile pid_param_t  Motor_Dir_A_pid;
volatile pid_param_t  ctrl_dis_pid;          //�ҵ������PID

volatile _pid_param_s   Servo_Cam_pid;          //����ͷ�������PID
volatile _pid_param_s   Servo_Adc_pid;          //��ж������PID

volatile adc_cbh adc={
        .A = 60,
        .B = 40,
        .C = 43
};


volatile flag_t Flag=
{

        .Adc_Stop = true,
        .left_break = false,
        .right_break = false,
        // ����������־λ
        .Begin_run = false,

        // �Ƿ񴿵����
        .Fix_adc = false,


        // �Ƿ�̶���ֵ
        .Fix_Thr = false,
        .Ostu_2 = false,
        .Fix_Thr_in_break = false,

        // �ؾ�
        .Ctrl_dis = false,

        // ����
        .Back = false,
        //ͣ��
        .Ui_Stop_Flag = 1,                          //ͣ����־λ
        .Duty_Stop_Flag = 1,
        .Global_Runing_Cnt = 0,                     //δͣ��ʱ maincontrol()ִ�д���
        .Adc_Stop_Flag = 0,

        //ͼ��
        .ui_open = false,                           //ͼ��ʹ��

        //�Ʋ�
        .Run_Distance = 0,                          //·�̼���ʹ��

        //��ʱ
        .Run_Time = 0,
        .Run_Time2 = 0,

        //ʶ�𵽶�·
        .break_Road = false,
        .break_Road_in = false,

        //Բ��
        .Big_Small_Ring = false,                    //��СԲ������

        .Left_Ring_Find = false,                    //Բ�����ڲ��߱�־λ
        .Left_Ring_Turn = false,                    //��Բ�����߱�־λ
        .Left_Ring_Out = false,                     //Բ���ڱ�־λ
        .Left_Ring_Out_Mend1 = false,               //�����̶���Ǳ�־λ
        .Left_Ring_Out_Mend2 = false,               //�������߱�־λ
        .Left_Ring = false,                         //Բ��ȫ�� ������������Ԫ�ش���

        .Right_Ring_Find = false,
        .Right_Ring_Turn = false,
        .Right_Ring_Out = false,
        .Right_Ring_Out_Mend1 = false,
        .Right_Ring_Out_Mend2 = false,
        .Right_Ring = false,
        //ʮ��
        .CrossRoad_Find = false,                    //ʮ��ʶ���־λ
        .CrossRoad_Block = false,                   //ʮ�����α�־λ

        .crossRight = false,                  //ʮ���Ҳ���

        .crossLeft = false,                    //ʮ���󲿷�
        .CrossIn = false,                      //����ʮ�ֱ�־λ
        .CrossOut = false,                      //����ʮ�ֱ�־λ

        //������
        .Garage_Out = true,                         //�����־λ
        .Garage_Find = false,                       //������ʶ���־λ
        .Garage_Go = false,                         //���������߱�־λ
        .Garage_In = false,                         //����־λ

        //�µ�
        .Ramp = false,                              //�µ���־λ

        //Ѱ��ģʽ�л���־λ
        .turnWAY_state = 0,

        //��¼����
        .Record = false,

        .Fork_In_Record = false,
        .Fork_Out_Record = false,
        .Left_Ring_Find_Record = false,
        .Left_Ring_Out_Record = false,
        .Right_Ring_Find_Record = false,
        .Right_Ring_Out_Record = false,
        .Garage_Record = false,
        .Ramp_Find_Record = false,
        .Ramp_Out_Record = false,

        //������
        .KaiHuan = false,

        .Fixed_Purpost_Speed = 0,
        .flag_straight_spd_up_func = 1,

        .car_speed_state = 0,

        // ·��
        .barricade = false,

};

/**************************************Բ������***********************************************/
volatile ring_cam_t ring_cam =
{
    .Left_Ring_Clc = 1,                 //Բ������
    .Right_Ring_Clc = 1,
    .Left_Ring_Type = 0,                //Բ�����ͣ�0��50�� 1��80��
    .Right_Ring_Type = 1,
    .StartTurn_Line = { 40, 36 },       //����ʱ��
    .Turn_Upslope = { 2, 1.4 },         //�����ϰ벿�ֲ���б�ʱ���
    .Out_Slope = { 2.05, 0.9 },          //��������б�ʱ���
    .Out_Angle = { 120, 80 },          //�����̶���ǽǶ�
    .Ring_AIM_DELTA = {5, 12},
};

/**************************************�µ�����***********************************************/
uint16_t Ramp_Dis_up = 1000;
uint16_t Ramp_pitch_1 = 12;
uint16_t Ramp_pitch_2 = 25;
uint16_t Ramp_pitch_3 = 12;
int16_t Ramp_Speed_Up = 120;
int16_t Ramp_Speed_Top = 120;
int8_t Ramp_Cnt = 1;
uint8_t ramp_task_state = 0;
uint8 Ramp_AimLine = 50;

/************************************** ·�ϲ���***********************************************/
uint16_t Barr_Dis_up = 1000;
uint16_t Barr_Fixd_Dis_up = 23;
short  barricade_Line = 12;
short bar_width = 20;
short bar_width2 = 10;
int8_t Barr_Direction = 0;           //����ⷽ��(1��ת��0��ת)
int8_t Barr_Cnt = 1;
uint8 barr_state = 0;

/**************************************�����***********************************************/
uint8_t Garage_Direction = 0;           //����ⷽ��(0��ת��1��ת)
uint8_t Garage_Clc = 1;
uint8_t Garage_Out = 1;  // �Ƿ����

int16_t Garage_Out_Time = 150;
int16_t Garage_Out_Imu_Angle = 50;
int16_t Garage_Out_Inner_Speed = 50;
int16_t Garage_Out_Outer_Speed = 100;
int16_t Garage_Out_Mid_Speed = 40;
int16_t Garage_Out_Angle_L = 90;
int16_t Garage_Out_Angle_R = 90;

int16_t Garage_Out_Time_2 = 150;

/***************************************��·**************************************************/
short Break_Line = 15;
int8_t Break_Cnt = 0;
short Bar_break_order = 0;
short Bar_break_count = 4;


/**************************************�������***********************************************/
volatile uint16_t CurrentServoDty = Servo_Center_Mid;          //�������ռ�ձ�
volatile servo_param_t servo_param=
{
        .Servo_Max = Servo_Delta,                   //������������  120
        .Ui_Servo_Mid = Servo_Center_Mid,               //�����ֵ 1130  1346
        .Ui_Adc_Thr = 0.5,

        .Ui_Adc_SP = 150.0f,                  //��д��P ����·��         0.9
        .Ui_Adc_SD = 480.0f,                  //��д��D����·��         3.5

        .Ui_Adc_Pk = 50.0f,
        .Ui_Adc_BP = 80.0f,                  //��д�ǣ��µ���
        .Ui_Adc_BD = 200.0f,                  //��д��D���µ���

        .Ui_Cam_SP = 2.65f,                 //����ͷP     3.6
        .Ui_Cam_SD = 10.0f,                //����ͷD    9

        .Servo_filter = 0.0f,
        .cam_servo_temp_1 = 0.0f,
        .cam_servo_temp_2 = 0.0f

};

/**************************************�������***********************************************/
volatile int16_t Left_Motor_Duty;               //����ռ�ձ�
volatile int16_t Rigt_Motor_Duty;               //�ҵ��ռ�ձ�
volatile motor_param_t motor_param=
{
        // ����PID
        .Ui_Dir_P = 70.0f,
        .Ui_Dir_I = 0.0f,
        .Ui_Dir_D = 500.0f,

        .Ui_Dir_A_P = 0.0f,
        .Ui_Dir_A_I = 0.0f,
        .Ui_Dir_A_D = 0.0f,

        //ֱ���������Ŀ���ٶ�
        .Ui_Speed = 120,               //������ʱ������ٶ�     75
        .Ui_Adc_Speed = 0,

        // ������ٱ�
        .Dece_K = 0.3f,

        // ����˥����
        .Begin_K = 0.6f,

        //PID
        .Ui_Motor_P = 25.0f,               //���P   5.0
        .Ui_Motor_I = 4.0f,               //���I   0.35p   2.5

        .Ui_Motor_Max_Out = 9999,          //����޷�
        .Ui_Dir_INT_Thr = 5000,

        .Ui_Diff_Max = 2500,
};

/**************************************��в���***********************************************/
volatile Err_Steering err_steering={ //���ƫ��ṹ�壨ʵ�Σ�
        .Err_x = 0,//���ƫ��ṹ�壨ʵ�Σ�
        .Err_h = 0,
        .Err_s = 0,
        .Err = 0,
        .Err_last = 0,
        .Err_x_last = 0,
        .Err_h_last = 0,
        .Errsum = 0,
        .Errdif = 0,
};

volatile L_State dg_state=
{
        .L_real = 0, //��1���
        .LM_real = 0, //��2���
        .RM_real = 0, //��1���
        .R_real = 0, //��2���


        .L_max = 3480, // max
        .LM_max = 3480,
        .RM_max = 3480,
        .R_max = 3350,

        .L_once = 0, //һ�ι�һ����
        .LM_once = 0,
        .RM_once = 0,
        .R_once = 0,

        .L_twice = 0, //���ι�һ����
        .LM_twice = 0,
        .RM_twice = 0,
        .R_twice = 0,
};



/**************************************���ƾ������***********************************************/

volatile ctrl_dis_param_t  dis_param=
{
    .ctrl_dis_P = 0.5,               //�ؾ�P
    .ctrl_dis_I = 0,               //�ؾ�I
    .ctrl_dis_D = 0,               //�ؾ�D

    .dis_Err = 0,
    .dis_Err_last = 0,

    .dis_aim = 600,
};

volatile short longStraightCount = 0;
short longStraightClearCount = 0;
volatile uint8 longStraightFlag = 0;

volatile bool longStraightAccelerate = 1;
volatile uint8_t longStr_Acc_Speed_1 = 10;
volatile uint8_t longStr_Acc_Speed_2 = 30;

