/*
 * variables.h
 *
 *  Created on: 2023年5月11日
 *      Author: ZGX
 */

#ifndef VARIABLES_H_
#define VARIABLES_H_
#include "zf_common_headfile.h"
#include "stdint.h"
#include "pid.h"
#include "stdbool.h"
/************************************处理结果***********************************************/
extern volatile short Cam_Process_Result;
extern uint8_t send_data_flag;      //标志位发送
extern uint8_t show_img_flag;          //标志显示图像
extern uint8_t pd_flag ;

/************************************PID结构体***********************************************/
extern volatile pid_param_t  Motor_Left_pid;
extern volatile pid_param_t  Motor_Rigt_pid;
extern volatile pid_param_t  ctrl_dis_pid;
extern volatile _pid_param_s   Servo_Adc_pid;
extern volatile _pid_param_s   Servo_Cam_pid;          //摄像头舵机控制PID
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


    // 发车软起动标志位
    volatile bool Begin_run;


    // 纯电磁
    volatile bool Fix_adc;


    // 固定阈值
    volatile bool Fix_Thr;
    volatile bool Ostu_2;
    volatile bool Fix_Thr_in_break;

    // 控距
    volatile int8_t Ctrl_dis;

    // 倒车
    volatile bool Back;
    //停车
    volatile uint32_t Global_Runing_Cnt;        //未停车时 main()执行次数
    volatile bool Ui_Stop_Flag;                 //停车标志位
    volatile bool Adc_Stop_Flag;

    volatile bool Duty_Stop_Flag;

    //图传
    volatile bool ui_open;                   //图传使能

    //计步
    volatile bool Run_Distance;            //路程计数使能

    //计时
    volatile bool Run_Time;               // 计时
    volatile bool Run_Time2;

    //断路
    volatile bool break_Road;
    volatile bool break_Road_in;

    //圆环
    volatile bool Big_Small_Ring;               //大小圆环交替

    volatile bool Left_Ring_Find;               //圆环出口补线标志位
    volatile bool Left_Ring_Turn;               //进圆环补线标志位
    volatile bool Left_Ring_Out;                //圆环内标志位
    volatile bool Left_Ring_Out_Mend1;          //出环固定打角标志位
    volatile bool Left_Ring_Out_Mend2;          //出环补线标志位
    volatile bool Left_Ring_OnlyOnce;           //圆环只跑一次
    volatile bool Left_Ring;                    //圆环全程 用于屏蔽其他元素处理

    volatile bool Right_Ring_Find;
    volatile bool Right_Ring_Turn;
    volatile bool Right_Ring_Out;
    volatile bool Right_Ring_Out_Mend1;
    volatile bool Right_Ring_Out_Mend2;
    volatile bool Right_Ring_OnlyOnce;
    volatile bool Right_Ring;

    //十字
    volatile bool CrossRoad_Find;               //十字识别标志位
    volatile bool CrossRoad_Block;              //十字屏蔽标志位
    volatile bool crossRight;                   //十字右部分

    volatile bool crossLeft;                    //十字左部分
    volatile bool CrossIn;                      //进入十字标志位
    volatile bool CrossOut;                      //进入十字标志位

    //斑马线
    volatile bool Garage_Out;                   //出库标志位
    volatile bool Garage_Find;                  //斑马线识别标志位
    volatile bool Garage_Go;                    //经过斑马线标志位
    volatile bool Garage_In;                    //入库标志位

    //坡道
    volatile bool Ramp;                         //坡道标志位

    //寻迹模式切换标志位
    volatile unsigned char turnWAY_state;                  //0：摄像头跑    1：丢线不多，用上次的摄像头误差跑      2：丢线太多 电感跑

    //记录赛道
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

    //开环跑
    volatile bool KaiHuan;

    volatile bool Fixed_Purpost_Speed;          // 固定速度标志位：1固定速度  2不固定速度

    volatile bool flag_straight_spd_up_func;


    volatile unsigned char car_speed_state;

    //路障

    volatile bool  barricade;

}flag_t;
extern volatile flag_t Flag;

/**************************************圆环参数***********************************************/
typedef struct
{
    volatile uint8_t Left_Ring_Clc;              //圆环跑过的次数
    volatile uint8_t Right_Ring_Clc;
    volatile uint8_t Left_Ring_Type;             //圆环类型（0：50， 1：80）
    volatile uint8_t Right_Ring_Type;
    volatile uint8_t StartTurn_Line[2];          //进环时机
    volatile float Turn_Upslope[2];              //进环上半部分补线斜率倍率
    volatile float Out_Slope[2];                 //出环补线斜率倍率
    volatile int16_t Out_Angle[2];          //出环固定打角角度
    volatile int8_t Ring_AIM_DELTA[2];      //圆环目标行整改

}ring_cam_t;
extern volatile ring_cam_t ring_cam;

/**************************************坡道参数***********************************************/
extern uint16_t Ramp_Dis_up;
extern uint16_t Ramp_pitch_1;
extern uint16_t Ramp_pitch_2;
extern uint16_t Ramp_pitch_3;
extern int16_t Ramp_Speed_Up;
extern int16_t Ramp_Speed_Top;
extern int8_t Ramp_Cnt;
extern uint8_t ramp_task_state;
extern uint8_t Ramp_AimLine;


/************************************** 路障参数***********************************************/
extern uint16_t Barr_Dis_up;
extern uint16_t Barr_Fixd_Dis_up;
extern short bar_width;
extern short bar_width2;
extern short  barricade_Line;
extern int8_t Barr_Cnt;
extern int8_t Barr_Direction;
extern uint8_t barr_state;



/**************************************库参数***********************************************/
extern uint8_t Garage_Direction;           //出入库方向
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


/************************************** 断路 ***********************************************/
extern int8_t Break_Cnt;
extern short Break_Line;
extern short Bar_break_count;
extern short Bar_break_order;


/**************************************舵机参数***********************************************/
extern volatile uint16_t CurrentServoDty;
typedef struct
{
    volatile short Ui_Servo_Mid;             //舵机中值
    volatile unsigned char Servo_Max;                  //舵机打角最大值

    volatile float Ui_Adc_SP;               //电感打角P
    volatile float Ui_Adc_SD;                //电感打角D
    volatile float Ui_Adc_BP;                //电感打角（大弯）
    volatile float Ui_Adc_BD;                //电感打角D（大弯）

    volatile float Ui_Cam_SP;               //摄像头P
    volatile float Ui_Cam_SD;                //摄像头D

    volatile float Ui_Adc_Pk;
    volatile float Ui_Adc_Thr;

    volatile float Servo_filter;
    volatile float cam_servo_temp_1;
    volatile float cam_servo_temp_2;

}servo_param_t;
extern volatile servo_param_t servo_param;

/**************************************电机参数***********************************************/
extern volatile int16_t Left_Motor_Duty;
extern volatile int16_t Rigt_Motor_Duty;
typedef struct
{
    volatile float Ui_Motor_P;               //电机P
    volatile float Ui_Motor_I;               //电机I

    volatile float Dece_K;
    volatile float Begin_K;

    volatile unsigned short Ui_Motor_Max_Out;             //输出限幅
    volatile unsigned short Ui_Dir_INT_Thr;

    volatile float Ui_Dir_P;               //电机P
    volatile float Ui_Dir_I;               //电机P
    volatile float Ui_Dir_D;               //电机P

    volatile float Ui_Dir_A_P;
    volatile float Ui_Dir_A_I;
    volatile float Ui_Dir_A_D;

    volatile short Ui_Speed;           //正常跑时的最大速度
    volatile short Ui_Adc_Speed;
    volatile short Ui_Diff_Max;         //差速最大值

}motor_param_t;
extern volatile motor_param_t motor_param;

/**************************************电感参数***********************************************/

typedef struct//电感状态结构体
{

    unsigned short L_real; //左1电感
    unsigned short LM_real; //左2电感
    unsigned short RM_real; //右1电感
    unsigned short R_real; //右2电感


    unsigned short L_max; // max
    unsigned short LM_max;
    unsigned short RM_max;
    unsigned short R_max;

    float L_once; //一次归一化后
    float LM_once;
    float RM_once;
    float R_once;

    float L_twice; //二次归一化后
    float LM_twice;
    float RM_twice;
    float R_twice;
} L_State;


typedef struct//位置式PID（舵机用）偏差值结构体
{
    float Err_x;//斜电感偏差
    float Err_h;//横电感偏差
    float Err_s;//竖电感偏差

    float Err;//根据当前电感输出计算出的总偏差值
    float Err_last;//用于存放上一次计算出的偏差值
    float Err_x_last;//用于存放上一次斜电感计算出的偏差值
    float Err_h_last;//用于存放上一次横电感计算出的偏差值
    float Errsum;//用于存放Err累加的和
    float Errdif;//用于存放Err的差分
}Err_Steering;

extern volatile Err_Steering err_steering;
extern volatile L_State dg_state;


/**************************************控制距离参数***********************************************/

typedef struct
{
    volatile float ctrl_dis_P;               //控距P
    volatile float ctrl_dis_I;               //控距I
    volatile float ctrl_dis_D;               //控距D

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
