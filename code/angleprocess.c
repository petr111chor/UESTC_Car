#include "zf_common_headfile.h"

/*****************************************************************
*Function: garage_out_process()
*Description: 开环出库，固定打角，固定路径长度
*parameter: *
*Return: *
*****************************************************************/
void garage_out_process()
{

    if (Garage_Out == 0)
    {
        return;
    }

    gpio_set_level(LED2, GPIO_LOW);
    if(Total_time > Garage_Out_Time)
    {
        Flag.Ui_Stop_Flag = 1;
        CHARGE_ON;

        // 防止蓝牙失效
        if(Total_time > Garage_Out_Time + Garage_Out_Time_2)
        {
            Flag.Run_Time = false;
            Total_time = 0;

            Flag.Garage_Out = false;
            Garage_Out = 0;
            // 准备发车
            Flag.Begin_run = 1;
            Flag.Ui_Stop_Flag = 0;
            Flag.Duty_Stop_Flag = 0;
        }




    }


}


void turn_Update(void)
{
    if(Flag.Ui_Stop_Flag)
        return;

    if (Flag.Right_Ring_Out_Mend1)
    {
        if (((ring_cam.Right_Ring_Type >> (ring_cam.Right_Ring_Clc-1 )) & 1) == 0)
            Flag.turnWAY_state = 2;
        else
            Flag.turnWAY_state = 4;
    }

    if (Flag.Left_Ring_Out_Mend1)
    {
        if (((ring_cam.Left_Ring_Type >> (ring_cam.Left_Ring_Clc-1 )) & 1) == 0)
            Flag.turnWAY_state = 2;
        else
            Flag.turnWAY_state = 4;
    }

    if(Flag.break_Road || Flag.Fix_adc)
    {
        Flag.turnWAY_state = 8;
    }

    if (Flag.Garage_Out && Garage_Out != 0)
    {
        if(eulerAngle.yaw >= Garage_Out_Imu_Angle || eulerAngle.yaw <= -Garage_Out_Imu_Angle)
        {
            gpio_set_level(LED2, GPIO_HIGH);
            Flag.turnWAY_state = 6;
        }else{
            Flag.turnWAY_state = 5;
        }
    }



    turn_Calculate_Cam();
    turn_final();
    //servo_param.cam_servo_temp_2 = servo_param.cam_servo_temp_1;
}



void ApplyDifferentialControl(float diff) {
    // 参数定义
    float base_speed = 10.0;       // 基准转速（转/秒）
    float ttt = 0.01;               // 采样时间（10ms）
    int PPR = 1;                // 编码器每圈脉冲数
    int max_speed=100;

    // 1. 目标速度差
    float target_speed_diff = diff; // 例如 diff=5.0 表示左轮比右轮快5转/秒

    // 2. 获取实际速度差
    int16_t left_pulses_origin = 0 ;
    int16_t right_pulses_origin= 0;
    int16_t left_pulses = encoder_get_count(TIM2_ENCODER);
    int16_t right_pulses = encoder_get_count(TIM4_ENCODER);
    float left_speed = (left_pulses-left_pulses_origin) / (ttt * PPR);
    float right_speed =(right_pulses-right_pulses_origin) / (ttt * PPR);
    float actual_speed_diff = left_speed - right_speed;
    left_pulses_origin=left_pulses;
    right_pulses_origin=right_pulses;

    // 3. PID计算
    float error = target_speed_diff - actual_speed_diff;
    float correction = PidLocCtrl(&Motor_Dir_pid, error);

    // 4. 调整左右轮速度
    float left_target = base_speed + correction;
    float right_target = base_speed - correction;

    // 5. 转换为PWM并输出
    int left_pwm = (left_target / max_speed) * 10000; // 假设max_speed=100转/秒对应PWM=10000
    int right_pwm = (right_target / max_speed) * 10000;
    MotorCtrl(left_pwm, right_pwm);
}



void turn_Calculate_Cam(void)
{
    // 断路时，用电感跑
    int diff_gain=3;
    if(Flag.turnWAY_state != 8)
    {
        //Cam_Process_Result = -(servo_param.Ui_Cam_SP * servo_param.cam_servo_temp_1
        //                   + servo_param.Ui_Cam_SD * (servo_param.cam_servo_temp_1 - servo_param.cam_servo_temp_2));
        Cam_Process_Result = servo_param.Servo_filter * diff_gain;
    }

}

void turn_final(void)
{
//    Cam_Process_Result=0;
    switch(Flag.turnWAY_state)
    {

        case 0:     //没丢线，摄像头跑
            //ServoCtrl(servo_param.Ui_Servo_Mid + Cam_Process_Result);
            ApplyDifferentialControl(Cam_Process_Result);
            break;
        case 1:     //丢线不多，用摄像头之前的值跑
            //ServoCtrl(servo_param.Ui_Servo_Mid + Cam_Process_Result);
            ApplyDifferentialControl(Cam_Process_Result);
            break;
        case 2:     //出环固定打角(50）
            if(Flag.Left_Ring)
                ServoCtrl(servo_param.Ui_Servo_Mid + ring_cam.Out_Angle[0]);
            else if(Flag.Right_Ring)
                ServoCtrl(servo_param.Ui_Servo_Mid - ring_cam.Out_Angle[0]);
            break;
        //case 3:     // 出环固定打角（一般）
        //    if (Flag.Left_Ring)
        //        ServoCtrl(servo_param.Ui_Servo_Mid - Fixd_turn_angle_nor);
        //    else if (Flag.Right_Ring)
        //        ServoCtrl(servo_param.Ui_Servo_Mid + Fixd_turn_angle_nor);
        //    break;
        case 4:     // 出环固定打角(80）
            if (Flag.Left_Ring)
                ServoCtrl(servo_param.Ui_Servo_Mid + ring_cam.Out_Angle[1]);
            else if (Flag.Right_Ring)
                ServoCtrl(servo_param.Ui_Servo_Mid - ring_cam.Out_Angle[1]);
            break;

        case 5:     // 出库固定打角
            if (Garage_Direction == 1)
                ServoCtrl(servo_param.Ui_Servo_Mid - Garage_Out_Angle_R); //固定打角
            if (Garage_Direction == 0)
                ServoCtrl(servo_param.Ui_Servo_Mid + Garage_Out_Angle_L); //固定打角
            break;

        case 6:     // 出库倒车
            ServoCtrl(servo_param.Ui_Servo_Mid - Cam_Process_Result);
            break;



    }

}





