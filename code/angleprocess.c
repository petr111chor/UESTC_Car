#include "zf_common_headfile.h"

/*****************************************************************
*Function: garage_out_process()
*Description: �������⣬�̶���ǣ��̶�·������
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

        // ��ֹ����ʧЧ
        if(Total_time > Garage_Out_Time + Garage_Out_Time_2)
        {
            Flag.Run_Time = false;
            Total_time = 0;

            Flag.Garage_Out = false;
            Garage_Out = 0;
            // ׼������
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
    // ��������
    float base_speed = 10.0;       // ��׼ת�٣�ת/�룩
    float ttt = 0.01;               // ����ʱ�䣨10ms��
    int PPR = 1;                // ������ÿȦ������
    int max_speed=100;

    // 1. Ŀ���ٶȲ�
    float target_speed_diff = diff; // ���� diff=5.0 ��ʾ���ֱ����ֿ�5ת/��

    // 2. ��ȡʵ���ٶȲ�
    int16_t left_pulses_origin = 0 ;
    int16_t right_pulses_origin= 0;
    int16_t left_pulses = encoder_get_count(TIM2_ENCODER);
    int16_t right_pulses = encoder_get_count(TIM4_ENCODER);
    float left_speed = (left_pulses-left_pulses_origin) / (ttt * PPR);
    float right_speed =(right_pulses-right_pulses_origin) / (ttt * PPR);
    float actual_speed_diff = left_speed - right_speed;
    left_pulses_origin=left_pulses;
    right_pulses_origin=right_pulses;

    // 3. PID����
    float error = target_speed_diff - actual_speed_diff;
    float correction = PidLocCtrl(&Motor_Dir_pid, error);

    // 4. �����������ٶ�
    float left_target = base_speed + correction;
    float right_target = base_speed - correction;

    // 5. ת��ΪPWM�����
    int left_pwm = (left_target / max_speed) * 10000; // ����max_speed=100ת/���ӦPWM=10000
    int right_pwm = (right_target / max_speed) * 10000;
    MotorCtrl(left_pwm, right_pwm);
}



void turn_Calculate_Cam(void)
{
    // ��·ʱ���õ����
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

        case 0:     //û���ߣ�����ͷ��
            //ServoCtrl(servo_param.Ui_Servo_Mid + Cam_Process_Result);
            ApplyDifferentialControl(Cam_Process_Result);
            break;
        case 1:     //���߲��࣬������ͷ֮ǰ��ֵ��
            //ServoCtrl(servo_param.Ui_Servo_Mid + Cam_Process_Result);
            ApplyDifferentialControl(Cam_Process_Result);
            break;
        case 2:     //�����̶����(50��
            if(Flag.Left_Ring)
                ServoCtrl(servo_param.Ui_Servo_Mid + ring_cam.Out_Angle[0]);
            else if(Flag.Right_Ring)
                ServoCtrl(servo_param.Ui_Servo_Mid - ring_cam.Out_Angle[0]);
            break;
        //case 3:     // �����̶���ǣ�һ�㣩
        //    if (Flag.Left_Ring)
        //        ServoCtrl(servo_param.Ui_Servo_Mid - Fixd_turn_angle_nor);
        //    else if (Flag.Right_Ring)
        //        ServoCtrl(servo_param.Ui_Servo_Mid + Fixd_turn_angle_nor);
        //    break;
        case 4:     // �����̶����(80��
            if (Flag.Left_Ring)
                ServoCtrl(servo_param.Ui_Servo_Mid + ring_cam.Out_Angle[1]);
            else if (Flag.Right_Ring)
                ServoCtrl(servo_param.Ui_Servo_Mid - ring_cam.Out_Angle[1]);
            break;

        case 5:     // ����̶����
            if (Garage_Direction == 1)
                ServoCtrl(servo_param.Ui_Servo_Mid - Garage_Out_Angle_R); //�̶����
            if (Garage_Direction == 0)
                ServoCtrl(servo_param.Ui_Servo_Mid + Garage_Out_Angle_L); //�̶����
            break;

        case 6:     // ���⵹��
            ServoCtrl(servo_param.Ui_Servo_Mid - Cam_Process_Result);
            break;



    }

}





