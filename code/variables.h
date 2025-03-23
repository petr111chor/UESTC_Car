/*
 * variables.h
 *
 *  Created on: 2023��5��11��
 *      Author: ZGX
 */

#ifndef VARIABLES_H_
#define VARIABLES_H_
#include "zf_common_headfile.h"
#include "stdint.h"
#include "pid.h"
#include "stdbool.h"
/************************************������***********************************************/
extern volatile short Cam_Process_Result;
extern uint8_t send_data_flag;      //��־λ����
extern uint8_t show_img_flag;          //��־��ʾͼ��
extern uint8_t pd_flag ;

/************************************PID�ṹ��***********************************************/
extern volatile pid_param_t  Motor_Left_pid;
extern volatile pid_param_t  Motor_Rigt_pid;
extern volatile pid_param_t  ctrl_dis_pid;
extern volatile _pid_param_s   Servo_Adc_pid;
extern volatile _pid_param_s   Servo_Cam_pid;          //����ͷ�������PID
extern volatile pid_param_t  Motor_Dir_pid;
extern volatile pid_param_t  Motor_Dir_A_pid;


typedef struct
{
    volatile float A;
    volatile float B;
    volatile float C;
}adc_cbh;

extern volatile adc_cbh adc;

//typedef unsigned char u8;
//typedef unsigned short u16;
//typedef unsigned int u32;


typedef struct
{
    volatile bool Adc_Stop;
    volatile bool left_break;
    volatile bool right_break;


    // �������𶯱�־λ
    volatile bool Begin_run;


    // �����
    volatile bool Fix_adc;


    // �̶���ֵ
    volatile bool Fix_Thr;
    volatile bool Ostu_2;
    volatile bool Fix_Thr_in_break;

    // �ؾ�
    volatile int8_t Ctrl_dis;

    // ����
    volatile bool Back;
    //ͣ��
    volatile uint32_t Global_Runing_Cnt;        //δͣ��ʱ main()ִ�д���
    volatile bool Ui_Stop_Flag;                 //ͣ����־λ
    volatile bool Adc_Stop_Flag;

    volatile bool Duty_Stop_Flag;

    //ͼ��
    volatile bool ui_open;                   //ͼ��ʹ��

    //�Ʋ�
    volatile bool Run_Distance;            //·�̼���ʹ��

    //��ʱ
    volatile bool Run_Time;               // ��ʱ
    volatile bool Run_Time2;

    //��·
    volatile bool break_Road;
    volatile bool break_Road_in;

    //Բ��
    volatile bool Big_Small_Ring;               //��СԲ������

    volatile bool Left_Ring_Find;               //Բ�����ڲ��߱�־λ
    volatile bool Left_Ring_Turn;               //��Բ�����߱�־λ
    volatile bool Left_Ring_Out;                //Բ���ڱ�־λ
    volatile bool Left_Ring_Out_Mend1;          //�����̶���Ǳ�־λ
    volatile bool Left_Ring_Out_Mend2;          //�������߱�־λ
    volatile bool Left_Ring_OnlyOnce;           //Բ��ֻ��һ��
    volatile bool Left_Ring;                    //Բ��ȫ�� ������������Ԫ�ش���

    volatile bool Right_Ring_Find;
    volatile bool Right_Ring_Turn;
    volatile bool Right_Ring_Out;
    volatile bool Right_Ring_Out_Mend1;
    volatile bool Right_Ring_Out_Mend2;
    volatile bool Right_Ring_OnlyOnce;
    volatile bool Right_Ring;

    //ʮ��
    volatile bool CrossRoad_Find;               //ʮ��ʶ���־λ
    volatile bool CrossRoad_Block;              //ʮ�����α�־λ
    volatile bool crossRight;                   //ʮ���Ҳ���

    volatile bool crossLeft;                    //ʮ���󲿷�
    volatile bool CrossIn;                      //����ʮ�ֱ�־λ
    volatile bool CrossOut;                      //����ʮ�ֱ�־λ

    //������
    volatile bool Garage_Out;                   //�����־λ
    volatile bool Garage_Find;                  //������ʶ���־λ
    volatile bool Garage_Go;                    //���������߱�־λ
    volatile bool Garage_In;                    //����־λ

    //�µ�
    volatile bool Ramp;                         //�µ���־λ

    //Ѱ��ģʽ�л���־λ
    volatile unsigned char turnWAY_state;                  //0������ͷ��    1�����߲��࣬���ϴε�����ͷ�����      2������̫�� �����

    //��¼����
    volatile bool Record;

    volatile bool Fork_In_Record;
    volatile bool Fork_Out_Record;
    volatile bool Left_Ring_Find_Record;
    volatile bool Left_Ring_Out_Record;
    volatile bool Right_Ring_Find_Record;
    volatile bool Right_Ring_Out_Record;
    volatile bool Garage_Record;
    volatile bool Ramp_Find_Record;
    volatile bool Ramp_Out_Record;

    //������
    volatile bool KaiHuan;

    volatile bool Fixed_Purpost_Speed;          // �̶��ٶȱ�־λ��1�̶��ٶ�  2���̶��ٶ�

    volatile bool flag_straight_spd_up_func;


    volatile unsigned char car_speed_state;

    //·��

    volatile bool  barricade;

}flag_t;
extern volatile flag_t Flag;

/**************************************Բ������***********************************************/
typedef struct
{
    volatile uint8_t Left_Ring_Clc;              //Բ���ܹ��Ĵ���
    volatile uint8_t Right_Ring_Clc;
    volatile uint8_t Left_Ring_Type;             //Բ�����ͣ�0��50�� 1��80��
    volatile uint8_t Right_Ring_Type;
    volatile uint8_t StartTurn_Line[2];          //����ʱ��
    volatile float Turn_Upslope[2];              //�����ϰ벿�ֲ���б�ʱ���
    volatile float Out_Slope[2];                 //��������б�ʱ���
    volatile int16_t Out_Angle[2];          //�����̶���ǽǶ�
    volatile int8_t Ring_AIM_DELTA[2];      //Բ��Ŀ��������

}ring_cam_t;
extern volatile ring_cam_t ring_cam;

/**************************************�µ�����***********************************************/
extern uint16_t Ramp_Dis_up;
extern uint16_t Ramp_pitch_1;
extern uint16_t Ramp_pitch_2;
extern uint16_t Ramp_pitch_3;
extern int16_t Ramp_Speed_Up;
extern int16_t Ramp_Speed_Top;
extern int8_t Ramp_Cnt;
extern uint8_t ramp_task_state;
extern uint8_t Ramp_AimLine;


/************************************** ·�ϲ���***********************************************/
extern uint16_t Barr_Dis_up;
extern uint16_t Barr_Fixd_Dis_up;
extern short bar_width;
extern short bar_width2;
extern short  barricade_Line;
extern int8_t Barr_Cnt;
extern int8_t Barr_Direction;
extern uint8_t barr_state;



/**************************************�����***********************************************/
extern uint8_t Garage_Direction;           //����ⷽ��
extern uint8_t Garage_Clc;

extern uint8_t Garage_Out;
extern int16_t Garage_Out_Time;
extern int16_t Garage_Out_Imu_Angle;
extern int16_t Garage_Out_Angle_L;
extern int16_t Garage_Out_Angle_R;
extern int16_t Garage_Out_Inner_Speed;
extern int16_t Garage_Out_Outer_Speed;
extern int16_t Garage_Out_Mid_Speed;

extern int16_t Garage_Out_Time_2;


/************************************** ��· ***********************************************/
extern int8_t Break_Cnt;
extern short Break_Line;
extern short Bar_break_count;
extern short Bar_break_order;


/**************************************�������***********************************************/
extern volatile uint16_t CurrentServoDty;
typedef struct
{
    volatile short Ui_Servo_Mid;             //�����ֵ
    volatile unsigned char Servo_Max;                  //���������ֵ

    volatile float Ui_Adc_SP;               //��д��P
    volatile float Ui_Adc_SD;                //��д��D
    volatile float Ui_Adc_BP;                //��д�ǣ����䣩
    volatile float Ui_Adc_BD;                //��д��D�����䣩

    volatile float Ui_Cam_SP;               //����ͷP
    volatile float Ui_Cam_SD;                //����ͷD

    volatile float Ui_Adc_Pk;
    volatile float Ui_Adc_Thr;

    volatile float Servo_filter;
    volatile float cam_servo_temp_1;
    volatile float cam_servo_temp_2;

}servo_param_t;
extern volatile servo_param_t servo_param;

/**************************************�������***********************************************/
extern volatile int16_t Left_Motor_Duty;
extern volatile int16_t Rigt_Motor_Duty;
typedef struct
{
    volatile float Ui_Motor_P;               //���P
    volatile float Ui_Motor_I;               //���I

    volatile float Dece_K;
    volatile float Begin_K;

    volatile unsigned short Ui_Motor_Max_Out;             //����޷�
    volatile unsigned short Ui_Dir_INT_Thr;

    volatile float Ui_Dir_P;               //���P
    volatile float Ui_Dir_I;               //���P
    volatile float Ui_Dir_D;               //���P

    volatile float Ui_Dir_A_P;
    volatile float Ui_Dir_A_I;
    volatile float Ui_Dir_A_D;

    volatile short Ui_Speed;           //������ʱ������ٶ�
    volatile short Ui_Adc_Speed;
    volatile short Ui_Diff_Max;         //�������ֵ

}motor_param_t;
extern volatile motor_param_t motor_param;

/**************************************��в���***********************************************/

typedef struct//���״̬�ṹ��
{

    unsigned short L_real; //��1���
    unsigned short LM_real; //��2���
    unsigned short RM_real; //��1���
    unsigned short R_real; //��2���


    unsigned short L_max; // max
    unsigned short LM_max;
    unsigned short RM_max;
    unsigned short R_max;

    float L_once; //һ�ι�һ����
    float LM_once;
    float RM_once;
    float R_once;

    float L_twice; //���ι�һ����
    float LM_twice;
    float RM_twice;
    float R_twice;
} L_State;


typedef struct//λ��ʽPID������ã�ƫ��ֵ�ṹ��
{
    float Err_x;//б���ƫ��
    float Err_h;//����ƫ��
    float Err_s;//�����ƫ��

    float Err;//���ݵ�ǰ���������������ƫ��ֵ
    float Err_last;//���ڴ����һ�μ������ƫ��ֵ
    float Err_x_last;//���ڴ����һ��б��м������ƫ��ֵ
    float Err_h_last;//���ڴ����һ�κ��м������ƫ��ֵ
    float Errsum;//���ڴ��Err�ۼӵĺ�
    float Errdif;//���ڴ��Err�Ĳ��
}Err_Steering;

extern volatile Err_Steering err_steering;
extern volatile L_State dg_state;


/**************************************���ƾ������***********************************************/

typedef struct
{
    volatile float ctrl_dis_P;               //�ؾ�P
    volatile float ctrl_dis_I;               //�ؾ�I
    volatile float ctrl_dis_D;               //�ؾ�D

    volatile float dis_Err;
    volatile float dis_Err_last;

    volatile int dis_aim;
}ctrl_dis_param_t;

extern volatile ctrl_dis_param_t  dis_param;

extern volatile short longStraightCount;
extern short longStraightClearCount;
extern volatile uint8_t longStraightFlag;

extern volatile bool longStraightAccelerate ;
extern volatile uint8_t longStr_Acc_Speed_1;
extern volatile uint8_t longStr_Acc_Speed_2;

#endif /* VARIABLES_H_ */
