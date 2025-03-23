#include "zf_common_headfile.h"

volatile float Purpost_Speed;           //目标速度
volatile short diff_speed;   //转向差速值
volatile short Purpost_Left_Speed;      //目标左轮速度
volatile short Purpost_Rigt_Speed;      //目标右轮速度
int16_t ecpulse1;
int16_t ecpulse2;
int16_t ecpulse;
float Enc_Total_Pos = 0.0;     //路程（编码器量纲）
int16_t Error1 = 0,Error2 = 0;
bool start_sum_dis;
float ALL_dis_sum = 0;
int dis_error;
int speed_delta;
int begin_time;
float break_rate;



void speed_Update(void)
{
    static int16_t ecpulse1_arr[4] = {0,0,0,0};
    static int16_t ecpulse2_arr[4] = {0,0,0,0};
    static int16_t last_ecpulse1;
    static int16_t last_ecpulse2;

    /*******************得到编码器的值（当前速度）******************************/
    ecpulse1 = -encoder_get_count(TIM4_ENCODER);                              // 左
    encoder_clear_count(TIM4_ENCODER);
    ecpulse2 = encoder_get_count(TIM6_ENCODER);                              // 右
    encoder_clear_count(TIM6_ENCODER);


    if(ecpulse1 > 500 || ecpulse1 < -300)
    {
       ecpulse1 = last_ecpulse1;
       ecpulse2 = last_ecpulse2;
    }

    /*******************对当前速度进行滤波（均值）******************************/
    for(int i = 3; i > 0; --i)
    {
        ecpulse1_arr[i] = ecpulse1_arr[i-1];
        ecpulse2_arr[i] = ecpulse2_arr[i-1];
    }

    ecpulse2_arr[0] = ecpulse2;
    ecpulse1_arr[0] = ecpulse1;

    ecpulse1 = (int16_t)((ecpulse1_arr[0] + ecpulse1_arr[1] + ecpulse1_arr[2] + ecpulse1_arr[3])/4);
    ecpulse2 = (int16_t)((ecpulse2_arr[0] + ecpulse2_arr[1] + ecpulse2_arr[2] + ecpulse2_arr[3])/4);

    last_ecpulse1 = ecpulse1;
    last_ecpulse2 = ecpulse2;


    /*******************更新目标速度******************************/

    Update_Purpost_Speed();

    //if (Flag.Ui_Stop_Flag == 1 )//停车
    //{
    //    Purpost_Left_Speed = 0;
    //    Purpost_Rigt_Speed = 0;
    //    Flag.Begin_run = 1;
        //Total_time = 0;
    //}

    //if(Flag.Ui_Stop_Flag == 0 && Flag.Begin_run == 1)
    //{
    //    Flag.Run_Time = 1;
    //    Purpost_Left_Speed = 0.7*Purpost_Left_Speed*Total_time/begin_time+0.3*Purpost_Left_Speed;
     //   Purpost_Rigt_Speed = 0.7*Purpost_Rigt_Speed*Total_time/begin_time+0.3*Purpost_Left_Speed;
     //   if(Total_time >= begin_time)
     //   {
     //       Flag.Run_Time = 0;
     //       Flag.Begin_run = 0;
     //       Total_time = 0;
     //   }
    //}

    Error1 = (int16)(Purpost_Left_Speed - ecpulse1);
    Error2 = (int16)(Purpost_Rigt_Speed - ecpulse2);

    Left_Motor_Duty = (short)Get_Left_Uk(Error1);
    Rigt_Motor_Duty = (short)Get_Right_Uk(Error2);

    LIMIT(Left_Motor_Duty, -8000, 8000);
    LIMIT(Rigt_Motor_Duty, -8000, 8000);

    //if (Flag.Duty_Stop_Flag == 1)//遥控停车
    //{
    //    Left_Motor_Duty = 0;
    //    Rigt_Motor_Duty = 0;
    //}

    MotorCtrl(-Left_Motor_Duty, -Rigt_Motor_Duty);
}

/*****************************************************************
*Function: Update_Purpost_Speed()
*Description: 更新目标速度
*parameter: 只在IRQ.C里面调用
*Return: *
*****************************************************************/
void Update_Purpost_Speed(void)
{
   // if(Flag.break_Road || Flag.Fix_adc)
    //{
      //  Purpost_Speed = motor_param.Ui_Adc_Speed;
     //   if(Flag.break_Road_in)
     //   {

     //      Purpost_Speed = (1-break_rate)*Purpost_Speed*(Total_time_2)/break_time + break_rate*Purpost_Speed;
     //   }
//        diff_speed = -(short)PidLocCtrl(&Motor_Dir_A_pid, err_steering.Err);
     //   diff_speed = 0;
    //}
    //else
    //{
        Purpost_Speed = 60;
        //if(longStraightFlag && longStraightAccelerate)
        //    Purpost_Speed = motor_param.Ui_Speed + longStr_Acc_Speed_2;
        //else if(longStraightCount > 0 && longStraightAccelerate)
        //    Purpost_Speed = motor_param.Ui_Speed + longStr_Acc_Speed_1;

        diff_speed = (short)PidLocCtrltest(&motor_pid_direction, 10);
    //}

    //if(Flag.Ramp)
    //{
    //    Purpost_Speed = Ramp_Speed_Up;
    //}


    speed_delta = (int)abs(diff_speed*0.3);
    Purpost_Left_Speed = Purpost_Speed - speed_delta + diff_speed;
    Purpost_Rigt_Speed = Purpost_Speed - speed_delta - diff_speed;


    //if (Flag.Garage_Out)  //出库固定差速
    //{
    //    if (Garage_Direction == 0)  // 左
    //    {
    //        Purpost_Left_Speed = -Garage_Out_Inner_Speed;
    //        Purpost_Rigt_Speed = -Garage_Out_Outer_Speed;
    //   }
    //    else if(Garage_Direction == 1) // 右
    //    {
    //        Purpost_Left_Speed = -Garage_Out_Outer_Speed;
    //        Purpost_Rigt_Speed = -Garage_Out_Inner_Speed;
    //    }

    //    if(eulerAngle.yaw >= Garage_Out_Imu_Angle || eulerAngle.yaw <= -Garage_Out_Imu_Angle)
    //    {
    //        Flag.Run_Time = true;
    //        Purpost_Rigt_Speed = -(int)(((Garage_Out_Time - Total_time)*1.0/Garage_Out_Time)*Garage_Out_Mid_Speed);
    //        Purpost_Left_Speed = -(int)(((Garage_Out_Time - Total_time)*1.0/Garage_Out_Time)*Garage_Out_Mid_Speed);
    //    }
    //}



}



//获得左轮控制量
float Get_Left_Uk(int16_t LeftEk)
{
    return PidIncCtrltest(&motor_pid_left, LeftEk);
}


//获得右轮控制量
float Get_Right_Uk(int16_t RightEk)
{
    return PidIncCtrltest(&motor_pid_right, RightEk);
}

//获得偏差控制量
float Get_delta(int16_t dis_error)
{
    return PidLocCtrltest(&ctrl_dis_pid, dis_error);
}
