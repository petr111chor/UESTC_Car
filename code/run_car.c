 /*
 * run_car.c
 *
 *  Created on: 2023��4��28��
 */

#include "zf_common_headfile.h"


volatile pid_param_t servo_pid;  // ���pid�ṹ�壨ʵ�Σ�
volatile pid_param_t motor_pid;  // ���pid�ṹ�壨ʵ�Σ�
int Total_time = 0;     //·�̣����������٣�
int Total_time_2 = 0;
int break_time;
int dm1xa_dis;
int8 dis_num;
float adc_p;


uint16 pit_ms_count = 0;
void run_car(void)
{
    // *********************** ���������  ************************
    if(0 == pit_ms_count % 10)                                                  // ÿ 40ms ��ȡһ�β����Ϣ ���� 40 ms Ƶ�� 25Hz
    {
        dl1b_get_distance();

    }

    pit_ms_count = (pit_ms_count == 995) ? (0) : (pit_ms_count + 5);            // 1000ms ���ڼ���

    // *********************** ��ʱ  ************************

    Total_time   = (Flag.Run_Time == 1) ? ( Total_time + 1)  : (0);
    Total_time_2 = (Flag.Run_Time2 == 1)? (Total_time_2 + 1) : (0);

    if (Garage_Out == true || Flag.barricade )
        get_yaw();

    if ( Flag.Ramp)
        get_pitch();


//    //���ͣ��
    if(!Flag.barricade && !Flag.Garage_Out && Flag.Adc_Stop)
    {
        if(adc_mean_filter_convert(L_ADC, 3) <=50 && adc_mean_filter_convert(LM_ADC, 3)<=50 && adc_mean_filter_convert(RM_ADC, 3)<=50 && adc_mean_filter_convert(R_ADC, 3) <= 50)
        {
            Flag.Ui_Stop_Flag = 1;
        }
    }


    // *********************** ��Ŷ������ ************************
    if(Flag.break_Road == 1 || Flag.Fix_adc)
    {
        if(Flag.break_Road == 1 || Flag.Fix_adc )
        {
            if(Break_Cnt == 0)
                Break_Cnt++;

            calculate_s((L_State*)&dg_state, (Err_Steering*)&err_steering);

            if(ABS(err_steering.Err) > servo_param.Ui_Adc_Thr)
                adc_p = servo_param.Ui_Adc_SP + ABS(err_steering.Err)*servo_param.Ui_Adc_Pk;
            else
                adc_p = servo_param.Ui_Adc_SP;

            Cam_Process_Result = (adc_p * err_steering.Err + servo_param.Ui_Adc_SD * err_steering.Errdif);
            ServoCtrl(servo_param.Ui_Servo_Mid + Cam_Process_Result);
        }
//        else if (Flag.Ramp == 1)
//        {
//            calculate_s((L_State*)&dg_state, (Err_Steering*)&err_steering);
//            Cam_Process_Result = (servo_param.Ui_Adc_BP * err_steering.Err + servo_param.Ui_Adc_BD * err_steering.Errdif);
//            ServoCtrl(servo_param.Ui_Servo_Mid + Cam_Process_Result);
//        }
    }
    // *********************** ������� ************************
    speed_Update();                //5ms �����������

}


/*
    if(0 == pit_ms_count % 20)
    {
        dm1xa_dis = dm1xa_receiver_ranging();

        if(Flag.Rear_go)
        {            if(dm1xa_dis > 1500 && dm1xa_dis != 6800)
            {
                bluetooth_ch9141_send_string("5go");
                Flag.Rear_go = false;
            }
        }

        if(!Flag.avoid_col)
        {
            if(Flag.Right_Ring_Find || Flag.Left_Ring_Find)
            {
                // �����������Բ��������Բ��,��ͣ��
                Flag.avoid_col = true;
                Flag.Ui_Stop_Flag = 1;
                beep_set_sound(BEEP_ON);
                gpio_low(LED1);

            }
        }else{
            if(dm1xa_dis < 1300)
            {
                // �����ͣ�µĹ����и��ܵ��˺󳵵���Ϯ��������
                Flag.Ui_Stop_Flag = 0;
                beep_set_sound(BEEP_OFF);
                gpio_high(LED1);
            }
            if(Flag.Right_Ring_Out || Flag.Left_Ring_Out)
            {
                // �÷�ײ��ʶ���ڳ���ʱ�� false����ֹ��Բ����Ƶ��ͣ��
                Flag.avoid_col = false;
            }
        }
         ******************** ��ײ ***********************************
          if(!Flag.avoid_col && !Flag.barricade && !Flag.Ramp && dm1xa_dis < 800)
              {
              dis_num++;
              if(dis_num == 3)
              {
                  beep_set_sound(BEEP_ON);
                  gpio_low(LED1);
                  dis_num = 0;
                  Flag.avoid_col = true;
                  Flag.Ui_Stop_Flag = 1;
              }
              }
          else if(Flag.avoid_col && dm1xa_dis > 800)
              {
              dis_num++;
              if(dis_num == 3)
              {
                  beep_set_sound(BEEP_OFF);
                  gpio_high(LED1);
                  dis_num = 0;
                  Flag.avoid_col = false;
                  Flag.Ui_Stop_Flag = 0;
              }
              }
    }

 */


