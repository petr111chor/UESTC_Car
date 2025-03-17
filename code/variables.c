    /*
 * variables.c
 *
 *  Created on: 2023年5月11日
 *      Author: ZGX
 */
#include "zf_common_headfile.h"
#include "motorServo.h"
/************************************处理结果******************************************/
volatile short Cam_Process_Result;

uint8_t send_data_flag = 0;//标志位发送

/************************************PID结构体***********************************************/
volatile pid_param_t  Motor_Left_pid;         //左电机控制PID
volatile pid_param_t  Motor_Rigt_pid;          //右电机控制PID
volatile pid_param_t  Motor_Dir_pid;
volatile pid_param_t  Motor_Dir_A_pid;
volatile pid_param_t  ctrl_dis_pid;          //右电机控制PID

volatile _pid_param_s   Servo_Cam_pid;          //摄像头舵机控制PID
volatile _pid_param_s   Servo_Adc_pid;          //电感舵机控制PID

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
        // 发车启动标志位
        .Begin_run = false,

        // 是否纯电磁跑
        .Fix_adc = false,


        // 是否固定阈值
        .Fix_Thr = false,
        .Ostu_2 = false,
        .Fix_Thr_in_break = false,

        // 控距
        .Ctrl_dis = false,

        // 倒车
        .Back = false,
        //停车
        .Ui_Stop_Flag = 1,                          //停车标志位
        .Duty_Stop_Flag = 1,
        .Global_Runing_Cnt = 0,                     //未停车时 maincontrol()执行次数
        .Adc_Stop_Flag = 0,

        //图传
        .ui_open = false,                           //图传使能

        //计步
        .Run_Distance = 0,                          //路程计数使能

        //计时
        .Run_Time = 0,
        .Run_Time2 = 0,

        //识别到断路
        .break_Road = false,
        .break_Road_in = false,

        //圆环
        .Big_Small_Ring = false,                    //大小圆环交替

        .Left_Ring_Find = false,                    //圆环出口补线标志位
        .Left_Ring_Turn = false,                    //进圆环补线标志位
        .Left_Ring_Out = false,                     //圆环内标志位
        .Left_Ring_Out_Mend1 = false,               //出环固定打角标志位
        .Left_Ring_Out_Mend2 = false,               //出环补线标志位
        .Left_Ring = false,                         //圆环全程 用于屏蔽其他元素处理

        .Right_Ring_Find = false,
        .Right_Ring_Turn = false,
        .Right_Ring_Out = false,
        .Right_Ring_Out_Mend1 = false,
        .Right_Ring_Out_Mend2 = false,
        .Right_Ring = false,
        //十字
        .CrossRoad_Find = false,                    //十字识别标志位
        .CrossRoad_Block = false,                   //十字屏蔽标志位

        .crossRight = false,                  //十字右部分

        .crossLeft = false,                    //十字左部分
        .CrossIn = false,                      //进入十字标志位
        .CrossOut = false,                      //进入十字标志位

        //斑马线
        .Garage_Out = true,                         //出库标志位
        .Garage_Find = false,                       //斑马线识别标志位
        .Garage_Go = false,                         //经过斑马线标志位
        .Garage_In = false,                         //入库标志位

        //坡道
        .Ramp = false,                              //坡道标志位

        //寻迹模式切换标志位
        .turnWAY_state = 0,

        //记录赛道
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

        //开环跑
        .KaiHuan = false,

        .Fixed_Purpost_Speed = 0,
        .flag_straight_spd_up_func = 1,

        .car_speed_state = 0,

        // 路障
        .barricade = false,

};

/**************************************圆环参数***********************************************/
volatile ring_cam_t ring_cam =
{
    .Left_Ring_Clc = 1,                 //圆环次数
    .Right_Ring_Clc = 1,
    .Left_Ring_Type = 0,                //圆环类型（0：50， 1：80）
    .Right_Ring_Type = 1,
    .StartTurn_Line = { 40, 36 },       //进环时机
    .Turn_Upslope = { 2, 1.4 },         //进环上半部分补线斜率倍率
    .Out_Slope = { 2.05, 0.9 },          //出环补线斜率倍率
    .Out_Angle = { 120, 80 },          //出环固定打角角度
    .Ring_AIM_DELTA = {5, 12},
};

/**************************************坡道参数***********************************************/
uint16_t Ramp_Dis_up = 1000;
uint16_t Ramp_pitch_1 = 12;
uint16_t Ramp_pitch_2 = 25;
uint16_t Ramp_pitch_3 = 12;
int16_t Ramp_Speed_Up = 120;
int16_t Ramp_Speed_Top = 120;
int8_t Ramp_Cnt = 1;
uint8_t ramp_task_state = 0;
uint8 Ramp_AimLine = 50;

/************************************** 路障参数***********************************************/
uint16_t Barr_Dis_up = 1000;
uint16_t Barr_Fixd_Dis_up = 23;
short  barricade_Line = 12;
short bar_width = 20;
short bar_width2 = 10;
int8_t Barr_Direction = 0;           //出入库方向(1左转，0右转)
int8_t Barr_Cnt = 1;
uint8 barr_state = 0;

/**************************************库参数***********************************************/
uint8_t Garage_Direction = 0;           //出入库方向(0左转，1右转)
uint8_t Garage_Clc = 1;
uint8_t Garage_Out = 1;  // 是否出库

int16_t Garage_Out_Time = 150;
int16_t Garage_Out_Imu_Angle = 50;
int16_t Garage_Out_Inner_Speed = 50;
int16_t Garage_Out_Outer_Speed = 100;
int16_t Garage_Out_Mid_Speed = 40;
int16_t Garage_Out_Angle_L = 90;
int16_t Garage_Out_Angle_R = 90;

int16_t Garage_Out_Time_2 = 150;

/***************************************断路**************************************************/
short Break_Line = 15;
int8_t Break_Cnt = 0;
short Bar_break_order = 0;
short Bar_break_count = 4;


/**************************************舵机参数***********************************************/
volatile uint16_t CurrentServoDty = Servo_Center_Mid;          //舵机设置占空比
volatile servo_param_t servo_param=
{
        .Servo_Max = Servo_Delta,                   //舵机左右最大打角  120
        .Ui_Servo_Mid = Servo_Center_Mid,               //舵机中值 1130  1346
        .Ui_Adc_Thr = 0.5,

        .Ui_Adc_SP = 150.0f,                  //电感打角P （断路）         0.9
        .Ui_Adc_SD = 480.0f,                  //电感打角D（断路）         3.5

        .Ui_Adc_Pk = 50.0f,
        .Ui_Adc_BP = 80.0f,                  //电感打角（坡道）
        .Ui_Adc_BD = 200.0f,                  //电感打角D（坡道）

        .Ui_Cam_SP = 2.65f,                 //摄像头P     3.6
        .Ui_Cam_SD = 10.0f,                //摄像头D    9

        .Servo_filter = 0.0f,
        .cam_servo_temp_1 = 0.0f,
        .cam_servo_temp_2 = 0.0f

};

/**************************************电机参数***********************************************/
volatile int16_t Left_Motor_Duty;               //左电机占空比
volatile int16_t Rigt_Motor_Duty;               //右电机占空比
volatile motor_param_t motor_param=
{
        // 差速PID
        .Ui_Dir_P = 70.0f,
        .Ui_Dir_I = 0.0f,
        .Ui_Dir_D = 500.0f,

        .Ui_Dir_A_P = 0.0f,
        .Ui_Dir_A_I = 0.0f,
        .Ui_Dir_A_D = 0.0f,

        //直道和弯道的目标速度
        .Ui_Speed = 120,               //正常跑时的最大速度     75
        .Ui_Adc_Speed = 0,

        // 弯道减速比
        .Dece_K = 0.3f,

        // 启动衰减比
        .Begin_K = 0.6f,

        //PID
        .Ui_Motor_P = 25.0f,               //电机P   5.0
        .Ui_Motor_I = 4.0f,               //电机I   0.35p   2.5

        .Ui_Motor_Max_Out = 9999,          //输出限幅
        .Ui_Dir_INT_Thr = 5000,

        .Ui_Diff_Max = 2500,
};

/**************************************电感参数***********************************************/
volatile Err_Steering err_steering={ //舵机偏差结构体（实参）
        .Err_x = 0,//舵机偏差结构体（实参）
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
        .L_real = 0, //左1电感
        .LM_real = 0, //左2电感
        .RM_real = 0, //右1电感
        .R_real = 0, //右2电感


        .L_max = 3480, // max
        .LM_max = 3480,
        .RM_max = 3480,
        .R_max = 3350,

        .L_once = 0, //一次归一化后
        .LM_once = 0,
        .RM_once = 0,
        .R_once = 0,

        .L_twice = 0, //二次归一化后
        .LM_twice = 0,
        .RM_twice = 0,
        .R_twice = 0,
};



/**************************************控制距离参数***********************************************/

volatile ctrl_dis_param_t  dis_param=
{
    .ctrl_dis_P = 0.5,               //控距P
    .ctrl_dis_I = 0,               //控距I
    .ctrl_dis_D = 0,               //控距D

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

