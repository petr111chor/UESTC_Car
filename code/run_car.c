 /*
 * run_car.c
 *
 *  Created on: 2023年4月28日
 */

#include "zf_common_headfile.h"


volatile pid_param_t servo_pid;  // 舵机pid结构体（实参）
volatile pid_param_t motor_pid;  // 电机pid结构体（实参）
int Total_time = 0;     //路程（编码器量纲）
int Total_time_2 = 0;
int break_time;
int dm1xa_dis;
int8 dis_num;
float adc_p;


uint16 pit_ms_count = 0;
void run_car(void)
{
    // *********************** 超声波测距  ************************
    if(0 == pit_ms_count % 10)                                                  // 每 40ms 获取一次测距信息 周期 40 ms 频率 25Hz
    {
        dl1b_get_distance();

    }

    pit_ms_count = (pit_ms_count == 995) ? (0) : (pit_ms_count + 5);            // 1000ms 周期计数

    // *********************** 计时  ************************

    Total_time   = (Flag.Run_Time == 1) ? ( Total_time + 1)  : (0);
    Total_time_2 = (Flag.Run_Time2 == 1)? (Total_time_2 + 1) : (0);

    if (Garage_Out == true || Flag.barricade )
        get_yaw();

    if ( Flag.Ramp)
        get_pitch();


//    //电磁停车
    if(!Flag.barricade && !Flag.Garage_Out && Flag.Adc_Stop)
    {
        if(adc_mean_filter_convert(L_ADC, 3) <=50 && adc_mean_filter_convert(LM_ADC, 3)<=50 && adc_mean_filter_convert(RM_ADC, 3)<=50 && adc_mean_filter_convert(R_ADC, 3) <= 50)
        {
            Flag.Ui_Stop_Flag = 1;
        }
    }


    // *********************** 电磁舵机控制 ************************
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
    // *********************** 电机控制 ************************
    speed_Update();                //5ms 电机控制周期

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
                // 如果发现了左圆环或者右圆环,就停下
                Flag.avoid_col = true;
                Flag.Ui_Stop_Flag = 1;
                beep_set_sound(BEEP_ON);
                gpio_low(LED1);

            }
        }else{
            if(dm1xa_dis < 1300)
            {
                // 如果在停下的过程中感受到了后车的来袭，就启动
                Flag.Ui_Stop_Flag = 0;
                beep_set_sound(BEEP_OFF);
                gpio_high(LED1);
            }
            if(Flag.Right_Ring_Out || Flag.Left_Ring_Out)
            {
                // 让防撞标识符在出环时赋 false，防止在圆环内频繁停车
                Flag.avoid_col = false;
            }
        }
         ******************** 防撞 ***********************************
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


