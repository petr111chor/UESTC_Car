                                                                                                                                                                                                                                                                /*
 * param_read_save.c
 *
 *  Created on: May 17, 2023
 *      Author: kern
 */

#include "zf_common_headfile.h"

//用于读取flash和写入flash的变量定义
uint32 flash_data_buffer[DATA_SIZE];
int16 imu_gyro_z_0 = 0;
int16 imu_gyro_x_0 = 0;
char txt2[32];

// **************************** 变量定义/2 ****************************

void data_load_to_buffer(void)
{
    // ***************** 基本设置 *************************
    flash_data_buffer[0] = (uint32)(Garage_Out*1000);
    flash_data_buffer[1] = (uint32)(Garage_Direction*1000);
    flash_data_buffer[2] = (uint32)(ring_cam.Left_Ring_Clc*1000);
    flash_data_buffer[3] = (uint32)(ring_cam.Right_Ring_Clc*1000);
    flash_data_buffer[4] = (uint32)(ring_cam.Left_Ring_Type*1000);
    flash_data_buffer[5] = (uint32)(ring_cam.Right_Ring_Type*1000);
    flash_data_buffer[6] = (uint32)(Bar_break_order*1000);
    flash_data_buffer[7] = (uint32)(Ramp_Cnt*1000);
    flash_data_buffer[8] = (uint32)(Barr_Cnt*1000);
    flash_data_buffer[9] = (uint32)(Garage_Clc*1000);


    // ***************** 电机参数 ***************************
    flash_data_buffer[10] = (uint32)(motor_param.Ui_Speed*1000);
    flash_data_buffer[11] = (uint32)(motor_param.Ui_Dir_P*1000);
    flash_data_buffer[12] = (uint32)(motor_param.Ui_Dir_D*1000);
    flash_data_buffer[13] = (uint32)(motor_param.Ui_Adc_Speed*1000);
    flash_data_buffer[14] = (uint32)(motor_param.Ui_Dir_A_P*1000);
    flash_data_buffer[15] = (uint32)(motor_param.Ui_Dir_A_D*1000);
    flash_data_buffer[16] = (uint32)(motor_param.Ui_Motor_P*1000);
    flash_data_buffer[17] = (uint32)(motor_param.Ui_Motor_I*1000);
    flash_data_buffer[18] = (uint32)(motor_param.Dece_K*1000);
    flash_data_buffer[19] = (uint32)(begin_time*1000);

    //***************** 舵机参数 ***************************
    flash_data_buffer[20] = (uint32)(servo_param.Ui_Cam_SP*1000);
    flash_data_buffer[21] = (uint32)(servo_param.Ui_Cam_SD*1000);
    flash_data_buffer[22] = (uint32)(servo_param.Ui_Adc_SP*1000);
    flash_data_buffer[23] = (uint32)(servo_param.Ui_Adc_SD*1000);
    flash_data_buffer[24] = (uint32)(servo_param.Ui_Adc_Pk*1000);
    flash_data_buffer[25] = (uint32)(servo_param.Ui_Adc_Thr*1000);
    flash_data_buffer[26] = (uint32)(Flag.Fix_adc*1000);
    flash_data_buffer[27] = (uint32)(Flag.Adc_Stop*1000);

    //***************** 图片参数 ***************************
    flash_data_buffer[30] = (uint32)(AIM_LINE_SET*1000);
    flash_data_buffer[31] = (uint32)(ring_cam.Ring_AIM_DELTA[0]*1000);
    flash_data_buffer[32] = (uint32)(ring_cam.Ring_AIM_DELTA[1]*1000);
    flash_data_buffer[33] = (uint32)(Flag.Fix_Thr*1000);
    flash_data_buffer[34] = (uint32)(Flag.Fix_Thr_in_break*1000);
    flash_data_buffer[35] = (uint32)(threshold*1000);
    flash_data_buffer[36] = (uint32)(Flag.Ostu_2*1000);
    flash_data_buffer[37] = (uint32)(light*1000);

    // *********************** 库参数   *********************************
    flash_data_buffer[40] = (uint32)(Garage_Out_Angle_L*1000);
    flash_data_buffer[41] = (uint32)(Garage_Out_Angle_R*1000);
    flash_data_buffer[42] = (uint32)(Garage_Out_Imu_Angle*1000);
    flash_data_buffer[43] = (uint32)(Garage_Out_Time*1000);
    flash_data_buffer[44] = (uint32)(Garage_Out_Inner_Speed*1000);
    flash_data_buffer[45] = (uint32)(Garage_Out_Outer_Speed*1000);
    flash_data_buffer[46] = (uint32)(Garage_Out_Mid_Speed*1000);
    flash_data_buffer[47] = (uint32)(Garage_Out_Time_2*1000);


    //******************* 路障参数  *************************************
    flash_data_buffer[50] = (uint32)(Barr_Direction*1000);
    flash_data_buffer[51] = (uint32)(Barr_Dis_up*1000);
    flash_data_buffer[52] = (uint32)(barricade_Line*1000);
    flash_data_buffer[53] = (uint32)(bar_width*1000);
    flash_data_buffer[54] = (uint32)(bar_width2*1000);
    flash_data_buffer[55] = (uint32)(Barr_Fixd_Dis_up*1000);

    //******************* 断路参数  *************************************
    flash_data_buffer[60] = (uint32)(Break_Line*1000);
    flash_data_buffer[61] = (uint32)(break_time*1000);
    flash_data_buffer[62] = (uint32)(break_rate*1000);
    flash_data_buffer[63] = (uint32)(adc.A*1000);
    flash_data_buffer[64] = (uint32)(adc.B*1000);
    flash_data_buffer[65] = (uint32)(adc.C*1000);
    flash_data_buffer[66] = (uint32)(dg_state.L_max*1000);
    flash_data_buffer[67] = (uint32)(dg_state.LM_max*1000);
    flash_data_buffer[68] = (uint32)(dg_state.RM_max*1000);
    flash_data_buffer[69] = (uint32)(dg_state.R_max*1000);

    //******************* 圆环参数 *************************************
    flash_data_buffer[70] = (uint32)(ring_cam.StartTurn_Line[0]*1000);
    flash_data_buffer[71] = (uint32)(ring_cam.Turn_Upslope[0]*1000);
    flash_data_buffer[72] = (uint32)(ring_cam.Out_Slope[0]*1000);
    flash_data_buffer[73] = (uint32)(ring_cam.Out_Angle[0]*1000);
    flash_data_buffer[74] = (uint32)(ring_cam.StartTurn_Line[1]*1000);
    flash_data_buffer[75] = (uint32)(ring_cam.Turn_Upslope[1]*1000);
    flash_data_buffer[76] = (uint32)(ring_cam.Out_Slope[1]*1000);
    flash_data_buffer[77] = (uint32)(ring_cam.Out_Angle[1]*1000);


    //****************** 坡道参数 ***********************************
    flash_data_buffer[80] = (uint32)(Ramp_Dis_up*1000);
    flash_data_buffer[81] = (uint32)(Ramp_AimLine*1000);
    flash_data_buffer[82] = (uint32)(Ramp_Speed_Up*1000);
    flash_data_buffer[83] = (uint32)(Ramp_pitch_1*1000);
    flash_data_buffer[84] = (uint32)(Ramp_pitch_2*1000);
    flash_data_buffer[85] = (uint32)(Ramp_pitch_3*1000);
    flash_data_buffer[86] = (uint32)(longStraightAccelerate*1000);
    flash_data_buffer[87] = (uint32)(longStr_Acc_Speed_1*1000);
    flash_data_buffer[88] = (uint32)(longStr_Acc_Speed_2*1000);
}


void ui_load_to_data(void)
{
    // ***************** 基本设置 ******************************
    Garage_Out                     = (uint8_t)(flash_data_buffer[0] / 1000);
    Garage_Direction               = (uint8_t)(flash_data_buffer[1] / 1000);
    ring_cam.Left_Ring_Clc         = (uint8_t)(flash_data_buffer[2] / 1000);
    ring_cam.Right_Ring_Clc        = (uint8_t)(flash_data_buffer[3] / 1000);
    ring_cam.Left_Ring_Type        = (uint8_t)(flash_data_buffer[4] / 1000);
    ring_cam.Right_Ring_Type       = (uint8_t)(flash_data_buffer[5] / 1000);
    Bar_break_order                = (short)(flash_data_buffer[6] / 1000);
    Ramp_Cnt                       = (int8_t)(flash_data_buffer[7] / 1000);
    Barr_Cnt                       = (int8_t)(flash_data_buffer[8] / 1000);
    Garage_Clc                     = (uint8_t)(flash_data_buffer[9] / 1000);


    // ***************** 电机参数 ***************************
    motor_param.Ui_Speed            = (short)(flash_data_buffer[10] / 1000);
    motor_param.Ui_Dir_P            = ((float)flash_data_buffer[11] / 1000.0f);
    motor_param.Ui_Dir_D            = ((float)flash_data_buffer[12] / 1000.0f);
    motor_param.Ui_Adc_Speed        = (short)(flash_data_buffer[13] / 1000);
    motor_param.Ui_Dir_A_P          = ((float)flash_data_buffer[14] / 1000.0f);
    motor_param.Ui_Dir_A_D          = ((float)flash_data_buffer[15] / 1000.0f);
    motor_param.Ui_Motor_P          = ((float)flash_data_buffer[16] / 1000.0f);
    motor_param.Ui_Motor_I          = ((float)flash_data_buffer[17] / 1000.0f);
    motor_param.Dece_K              = ((float)flash_data_buffer[18] / 1000.0f);
    begin_time                      = (int)(flash_data_buffer[19] / 1000.0f);

    //***************** 舵机参数 ***************************
    servo_param.Ui_Cam_SP          = ((float)flash_data_buffer[20] / 1000.0f);
    servo_param.Ui_Cam_SD          = ((float)flash_data_buffer[21] / 1000.0f);
    servo_param.Ui_Adc_SP          = ((float)flash_data_buffer[22] / 1000.0f);
    servo_param.Ui_Adc_SD          = ((float)flash_data_buffer[23] / 1000.0f);
    servo_param.Ui_Adc_Pk          = ((float)flash_data_buffer[24] / 1000.0f);
    servo_param.Ui_Adc_Thr         = ((float)flash_data_buffer[25] / 1000.0f);
    Flag.Fix_adc                   = (bool)(flash_data_buffer[26] / 1000);
    Flag.Adc_Stop                  = (bool)(flash_data_buffer[27] / 1000);

    //***************** 图片参数 ***************************
    AIM_LINE_SET                   = (uint8_t)(flash_data_buffer[30] / 1000);
    ring_cam.Ring_AIM_DELTA[0]     = (uint8_t)(flash_data_buffer[31] / 1000);
    ring_cam.Ring_AIM_DELTA[1]     = (uint8_t)(flash_data_buffer[32] / 1000);
    Flag.Fix_Thr                   = (bool)(flash_data_buffer[33] / 1000);
    Flag.Fix_Thr_in_break          = (bool)(flash_data_buffer[34] / 1000);
    threshold                      = (uint16)(flash_data_buffer[35] / 1000);
    Flag.Ostu_2                    = (bool)(flash_data_buffer[36] / 1000);
    light                          = (uint16)(flash_data_buffer[37] / 1000);

    //******************* 库参数  *************************************
    Garage_Out_Angle_L             = (int16_t)(flash_data_buffer[40] / 1000);
    Garage_Out_Angle_R             = (int16_t)(flash_data_buffer[41] / 1000);
    Garage_Out_Imu_Angle           = (int16_t)(flash_data_buffer[42] / 1000);
    Garage_Out_Time                = (int16_t)(flash_data_buffer[43] / 1000);
    Garage_Out_Inner_Speed         = (int16_t)(flash_data_buffer[44] / 1000);
    Garage_Out_Outer_Speed         = (int16_t)(flash_data_buffer[45] / 1000);
    Garage_Out_Mid_Speed           = (int16_t)(flash_data_buffer[46] / 1000);
    Garage_Out_Time_2              = (int16_t)(flash_data_buffer[47] / 1000);


    //******************* 路障参数  *************************************
    Barr_Direction                 = (int8_t)(flash_data_buffer[50] / 1000);
    Barr_Dis_up                    = (uint16_t)(flash_data_buffer[51] / 1000);
    barricade_Line                 = (short)(flash_data_buffer[52] / 1000);
    bar_width                      = (short)(flash_data_buffer[53] / 1000);
    bar_width2                     = (short)(flash_data_buffer[54] / 1000);
    Barr_Fixd_Dis_up               = (uint16_t)(flash_data_buffer[55] / 1000);

    //******************* 断路参数  *************************************
    Break_Line                     = (int)(flash_data_buffer[60] / 1000.0f);
    break_time                     = (int)(flash_data_buffer[61] / 1000.0f);
    break_rate                     = ((float)flash_data_buffer[62] / 1000.0f);
    adc.A                          = ((float)flash_data_buffer[63] / 1000.0f);
    adc.B                          = ((float)flash_data_buffer[64] / 1000.0f);
    adc.C                          = ((float)flash_data_buffer[65] / 1000.0f);
    dg_state.L_max                 = (unsigned short)(flash_data_buffer[66] / 1000);
    dg_state.LM_max                = (unsigned short)(flash_data_buffer[67] / 1000);
    dg_state.RM_max                = (unsigned short)(flash_data_buffer[68] / 1000);
    dg_state.R_max                 = (unsigned short)(flash_data_buffer[69] / 1000);

    //****************** 圆环参数 ********************************************
    ring_cam.StartTurn_Line[0]     = (uint8_t)(flash_data_buffer[70] / 1000);
    ring_cam.Turn_Upslope[0]       = ((float)flash_data_buffer[71] / 1000.0f);
    ring_cam.Out_Slope[0]          = ((float)flash_data_buffer[72] / 1000.0f);
    ring_cam.Out_Angle[0]          = (int16_t)(flash_data_buffer[73] / 1000);
    ring_cam.StartTurn_Line[1]     = (uint8_t)(flash_data_buffer[74] / 1000);
    ring_cam.Turn_Upslope[1]       = ((float)flash_data_buffer[75] / 1000.0f);
    ring_cam.Out_Slope[1]          = ((float)flash_data_buffer[76] / 1000.0f);
    ring_cam.Out_Angle[1]          = (int16_t)(flash_data_buffer[77] / 1000);


    //****************** 坡道参数 ***********************************
    Ramp_Dis_up                    = (uint16_t)(flash_data_buffer[80] / 1000);
    Ramp_AimLine                   = (uint8)(flash_data_buffer[81] / 1000);
    Ramp_Speed_Up                  = (int16_t)(flash_data_buffer[82] / 1000);
    Ramp_pitch_1                   = (uint16_t)(flash_data_buffer[83]/ 1000);
    Ramp_pitch_2                   = (uint16_t)(flash_data_buffer[84]/ 1000);
    Ramp_pitch_3                   = (uint16_t)(flash_data_buffer[85]/ 1000);
    longStraightAccelerate         = (int8)(flash_data_buffer[86] / 1000);
    longStr_Acc_Speed_1            = (uint8)(flash_data_buffer[87] / 1000);
    longStr_Acc_Speed_2            = (uint8)(flash_data_buffer[88] / 1000);

}



void ui_load_to_buffer(void)
{
    // ***************** 基本设置 ***************************
    for (int i = 0; i < 10; i++)
    {
        flash_data_buffer[i] = (uint32)(UI_Param[0][i] * 1000);
        flash_data_buffer[10+i] = (uint32)(UI_Param[1][i] * 1000);
        flash_data_buffer[20+i] = (uint32)(UI_Param[2][i] * 1000);
        flash_data_buffer[30+i] = (uint32)(UI_Param[3][i] * 1000);
        flash_data_buffer[40+i] = (uint32)(UI_Param[4][i] * 1000);
        flash_data_buffer[50+i] = (uint32)(UI_Param[5][i] * 1000);
        flash_data_buffer[60+i] = (uint32)(UI_Param[6][i] * 1000);
        flash_data_buffer[70+i] = (uint32)(UI_Param[7][i] * 1000);
        flash_data_buffer[80+i] = (uint32)(UI_Param[8][i] * 1000);
    }
}



void buffer_load_to_ui(void)
{
    // ***************** 基本设置 ***************************
    for (int i = 0; i < 10; i++)
    {
        UI_Param[0][i] = (float)flash_data_buffer[i] / 1000.0;
        UI_Param[1][i] = (float)flash_data_buffer[10+i] / 1000.0;
        UI_Param[2][i] = (float)flash_data_buffer[20+i] / 1000.0;
        UI_Param[3][i] = (float)flash_data_buffer[30+i] / 1000.0;
        UI_Param[4][i] = (float)flash_data_buffer[40+i] / 1000.0;
        UI_Param[5][i] = (float)flash_data_buffer[50+i] / 1000.0;
        UI_Param[6][i] = (float)flash_data_buffer[60+i] / 1000.0;
        UI_Param[7][i] = (float)flash_data_buffer[70+i] / 1000.0;
        UI_Param[8][i] = (float)flash_data_buffer[80+i] / 1000.0;
    }
}


/*****************************************************************
*Function: gps_save_write()
*Description: 将存储的gps数据写入flash
*parameter: *
*Return: *
*****************************************************************/
void param_save(bool is_ui)
{
    if(is_ui)
    {
        ui_load_to_buffer();
    }
    else
    {
        data_load_to_buffer();
    }

    flash_erase_page(SECTION_INDEX, PAGE_INDEX);                              // 擦掉数据
    flash_write_page(SECTION_INDEX, PAGE_INDEX, flash_data_buffer, DATA_SIZE);      // 把缓冲区的数据写回去

}

void get_homodyne(void)//获取零差
{
    oled_show_string(60, 0, "ok");
    system_delay_ms(2000);

    gpio_low(LED1);
    for(int i = 0;i<10000;i++)
    {
        imu963ra_get_gyro();
        GyroOffset.z += imu963ra_gyro_z;
        GyroOffset.x += imu963ra_gyro_x;
    }
    GyroOffset.z /= 10000;
    GyroOffset.x /= 10000;

   gpio_high(LED1);

    sprintf(txt2, "z-homodyne: %f", GyroOffset.z);
    oled_show_string(2, 2, txt2);
    sprintf(txt2, "x-homodyne: %f", GyroOffset.x);
    oled_show_string(2, 3, txt2);
    while(1)
    {
        if(key_get_state(KEY_1) == KEY_LONG_PRESS)
        {
            oled_clear();
            GyroOffset.z = 0;
            GyroOffset.x = 0;
             return;
        }

        if(key_get_state(KEY_3) == KEY_LONG_PRESS)
        {
            oled_clear();
            break;
        }

    }
}


/*****************************************************************
*Function: gps_save_read()
*Description: 将存储的gps数据从flash中读出，并写入gps_list坐标列表中  等待gps_turn.c中程序调用
*parameter: *
*Return: *
*****************************************************************/
void param_read(void)
{
    flash_read_page(SECTION_INDEX, PAGE_INDEX, flash_data_buffer, DATA_SIZE);           // 读出来已有的数据

    buffer_load_to_ui();
}



/*****************************************************************
*Function: Data_init()
*Description: 在UI开启前根据按下键的情况将数据初始化
*parameter: *
*Return: *
*****************************************************************/
short Data_init(void)
{
    key_state_enum res_0;
    //res_0 = key_Scan();
    if(key_get_state(KEY_1) == KEY_SHORT_PRESS)
    {
        Flag.Duty_Stop_Flag = 1;
        return 0;
    }
    else if (key_get_state(KEY_2) == KEY_SHORT_PRESS)
    {
        oled_clear();
        mt9v03x_test();
        mt9v03x_test_2();
        oled_show_binary_image(OLED_MID, 0, (const uint8 *) &gImage_seekfree_logo, 48, 48 ,48, 48);
        oled_show_string(OLED_MID, 7, "UESTC's Baton");
        return 1;
    }
    else if (key_get_state(KEY_3) == KEY_SHORT_PRESS)
    {
        oled_clear();
        ele_mad_test();
        get_homodyne();
        oled_show_binary_image(OLED_MID, 0, (const uint8 *) &gImage_seekfree_logo, 48, 48, 48, 48);
        oled_show_string(OLED_MID, 7, "UESTC's Baton");
        return 1;
    }
    else if (key_get_state(KEY_2) == KEY_LONG_PRESS)   //默认模式 这里有问题
        return 1;
    else if (key_get_state(KEY_1) == KEY_LONG_PRESS)    //低速模式
    {
        gpio_set_level(LED1, GPIO_LOW);
        //***************** 基本设置 ***************************
        Garage_Out                     = 1;
        Garage_Direction               = 1;
        ring_cam.Left_Ring_Clc         = 1;
        ring_cam.Right_Ring_Clc        = 1;
        ring_cam.Left_Ring_Type        = 1;                //圆环类型（0：50， 1：80）
        ring_cam.Right_Ring_Type       = 1;
        Bar_break_order                = 15;
        Ramp_Cnt                       = 1;
        Barr_Cnt                       = 1;
        Garage_Clc                     = 1;

        //***************** 电机参数 ***************************
           motor_param.Ui_Speed     = 100;
           motor_param.Ui_Dir_P     = 0.5f;
           motor_param.Ui_Dir_D     = 0.4f;
           motor_param.Ui_Adc_Speed = 100;
           motor_param.Ui_Dir_A_P   = 40.0f;
           motor_param.Ui_Dir_A_D   = 150.0f;
           motor_param.Ui_Motor_P  = 20.0f;
           motor_param.Ui_Motor_I  = 4.0f;
           motor_param.Dece_K      = 0.05f;
           begin_time              = 50;

           //***************** 舵机参数 ***************************
           servo_param.Ui_Cam_SP = 2.4;
           servo_param.Ui_Cam_SD = 9.1;
           servo_param.Ui_Adc_SP = 55;
           servo_param.Ui_Adc_SD = 550;
           servo_param.Ui_Adc_Pk = 70;
           servo_param.Ui_Adc_Thr         = 0.5;
           Flag.Fix_adc                   = 0;
           Flag.Adc_Stop                  = 0;


           //***************** 图片参数 ***************************
           AIM_LINE_SET                   = 35;
           ring_cam.Ring_AIM_DELTA[0]     = 0;
           ring_cam.Ring_AIM_DELTA[1]     = 5;
           Flag.Fix_Thr                   = 0;
           Flag.Fix_Thr_in_break          = 1;
           threshold                      = 130;
           Flag.Ostu_2                    = 0;
           light                          = 120;

           /*****************库参数*****************************/
           Garage_Out_Angle_L             = 57;//55
           Garage_Out_Angle_R             = 56;//55
           Garage_Out_Imu_Angle           = 63;//66
           Garage_Out_Time                = 180;
           Garage_Out_Inner_Speed         = 20;
           Garage_Out_Outer_Speed         = 70;
           Garage_Out_Mid_Speed           = 40;
           Garage_Out_Time_2                = 500;


           //******************* 路障参数  *************************************
           Barr_Direction                 = 1;//右
           Barr_Dis_up                    = 700;
           barricade_Line                 = 14;
           bar_width                      = 32;
           bar_width2                     = 15;
           Barr_Fixd_Dis_up               = 50;


           //******************* 断路参数  *************************************
           Break_Line                    = 14;
           break_time                    = 150;
           break_rate                    = 1;
           adc.A                         = 20;
           adc.B                         = 15;
           adc.C                         = 16;
           dg_state.L_max                = 3309;
           dg_state.LM_max               = 3052;
           dg_state.RM_max               = 3189;
           dg_state.R_max                = 3294;


           //****************** 圆环参数 ********************************************
           ring_cam.StartTurn_Line[0] = 40;
           ring_cam.Turn_Upslope[0] = 2.3;
           ring_cam.Out_Slope[0] = 2.14;
           ring_cam.Out_Angle[0] = 50;          //出环固定打角角度

           ring_cam.StartTurn_Line[1] = 39;
           ring_cam.Turn_Upslope[1] = 1.3;
           ring_cam.Out_Slope[1] = 1.8;
           ring_cam.Out_Angle[1] = 30;



           //****************** 坡道参数 ***********************************
           Ramp_Dis_up                    = 600;
           Ramp_AimLine                   = 50;
           Ramp_Speed_Up                  = 100;
           Ramp_pitch_1                   = 10;
           Ramp_pitch_2                   = 10;
           Ramp_pitch_3                   = 2;
           longStraightAccelerate         = 0;
           longStr_Acc_Speed_1            = 10;
           longStr_Acc_Speed_2            = 15;


           servo_param.Servo_Max          = 90;
           servo_param.Ui_Servo_Mid       = 730;

        param_save(0);
        gpio_set_level(LED1, GPIO_HIGH);
              return 1;
    }
    else if (key_get_state(KEY_2) == KEY_LONG_PRESS)   //中速模式
    {
        gpio_set_level(LED1, GPIO_LOW);

        //***************** 基本设置 ***************************
        Garage_Out                     = 1;
        Garage_Direction               = 1;
        ring_cam.Left_Ring_Clc         = 1;
        ring_cam.Right_Ring_Clc        = 1;
        ring_cam.Left_Ring_Type        = 1;                //圆环类型（0：50， 1：80）
        ring_cam.Right_Ring_Type       = 1;
        Bar_break_order                = 15;
        Ramp_Cnt                       = 1;
        Barr_Cnt                       = 1;
        Garage_Clc                     = 1;

        //***************** 电机参数 ***************************
           motor_param.Ui_Speed     = 120;
           motor_param.Ui_Dir_P     = 0.5f;
           motor_param.Ui_Dir_D     = 0.4f;
           motor_param.Ui_Adc_Speed = 100;
           motor_param.Ui_Dir_A_P   = 40.0f;
           motor_param.Ui_Dir_A_D   = 150.0f;
           motor_param.Ui_Motor_P  = 20.0f;
           motor_param.Ui_Motor_I  = 4.0f;
           motor_param.Dece_K      = 0.05f;
           begin_time              = 100;

           //***************** 舵机参数 ***************************
           servo_param.Ui_Cam_SP = 2.4;
           servo_param.Ui_Cam_SD = 9.1;
           servo_param.Ui_Adc_SP = 50;
           servo_param.Ui_Adc_SD = 500;
           servo_param.Ui_Adc_Pk = 70;
           servo_param.Ui_Adc_Thr         = 0.5;
           Flag.Fix_adc                   = 0;
           Flag.Adc_Stop                  = 0;


           //***************** 图片参数 ***************************
           AIM_LINE_SET                   = 32;
           ring_cam.Ring_AIM_DELTA[0]     = 0;
           ring_cam.Ring_AIM_DELTA[1]     = 4;
           Flag.Fix_Thr                   = 0;
           Flag.Fix_Thr_in_break          = 1;
           threshold                      = 130;
           Flag.Ostu_2                    = 0;
           light                          = 120;

           /*****************库参数*****************************/
           Garage_Out_Angle_L             = 57;//55
           Garage_Out_Angle_R             = 56;//55
           Garage_Out_Imu_Angle           = 63;//66
           Garage_Out_Time                = 180;
           Garage_Out_Inner_Speed         = 20;
           Garage_Out_Outer_Speed         = 70;
           Garage_Out_Mid_Speed           = 40;
           Garage_Out_Time_2              = 500;


           //******************* 路障参数  *************************************
           Barr_Direction                 = 1;//右
           Barr_Dis_up                    = 800;
           barricade_Line                 = 14;
           bar_width                      = 30;
           bar_width2                     = 10;
           Barr_Fixd_Dis_up               = 45;


           //******************* 断路参数  *************************************
           Break_Line                    = 14;
           break_time                    = 150;
           break_rate                    = 0.6;
           adc.A                         = 20;
           adc.B                         = 15;
           adc.C                         = 16;
           dg_state.L_max                = 3309;
           dg_state.LM_max               = 3052;
           dg_state.RM_max               = 3189;
           dg_state.R_max                = 3294;


           //****************** 圆环参数 ********************************************
           ring_cam.StartTurn_Line[0] = 40;
           ring_cam.Turn_Upslope[0] = 2.3;
           ring_cam.Out_Slope[0] = 2.14;
           ring_cam.Out_Angle[0] = 50;          //出环固定打角角度

           ring_cam.StartTurn_Line[1] = 39;
           ring_cam.Turn_Upslope[1] = 1.3;
           ring_cam.Out_Slope[1] = 1.8;
           ring_cam.Out_Angle[1] = 30;



           //****************** 坡道参数 ***********************************
           Ramp_Dis_up                    = 600;
           Ramp_AimLine                   = 50;
           Ramp_Speed_Up                  = 120;
           Ramp_pitch_1                   = 10;
           Ramp_pitch_2                   = 10;
           Ramp_pitch_3                   = 2;
           longStraightAccelerate         = 0;
           longStr_Acc_Speed_1            = 10;
           longStr_Acc_Speed_2            = 15;


           servo_param.Servo_Max          = 90;
           servo_param.Ui_Servo_Mid       = 730;

        param_save(0);
        gpio_set_level(LED1, GPIO_HIGH);
        return 1;
    }
    else if (key_get_state(KEY_3) == KEY_LONG_PRESS)    //高速模式
    {
        gpio_set_level(LED1, GPIO_LOW);

        //***************** 电机参数 ***************************

         //直道和弯道的目标速度
        //直道和弯道的目标速度
       motor_param.Ui_Speed            = 160;

        //PID
        motor_param.Ui_Motor_P = 20.0f;               //电机P   5.0
        motor_param.Ui_Motor_I = 4.0f;               //电机I   0.35p

        motor_param.Ui_Dir_P    = 70.0f;
        motor_param.Ui_Dir_I    = 0.0f;
        motor_param.Ui_Dir_D    = 500.0f;

         //***************** 舵机参数 ***************************
         servo_param.Ui_Cam_SP = 2.90;
         servo_param.Ui_Cam_SD = 10.0;

         servo_param.Ui_Adc_SP = 160;
         servo_param.Ui_Adc_SD = 720;

         servo_param.Servo_Max          = 90;
         servo_param.Ui_Servo_Mid       = 730;
         servo_param.Ui_Adc_BP          = 80;
         servo_param.Ui_Adc_BD          = 200;

 //        //***************** 图片参数 ***************************
         AIM_LINE_SET                   = 31;
         ring_cam.Ring_AIM_DELTA[0]     = 7;
         ring_cam.Ring_AIM_DELTA[1]     = 20;
         Flag.Fix_Thr                   = 0;
         Flag.Fix_Thr_in_break          = 0;
         threshold                      = 80;
         Flag.Ostu_2                    = 0;
 //
 //        //****************** 坡道参数 ***********************************
         Ramp_Dis_up                    = 1000;
         Ramp_pitch_1                   = 12;
         Ramp_pitch_2                   = 25;
         Ramp_pitch_3                   = 12;
         Ramp_Speed_Up                  = 110;
         Ramp_Speed_Top                 = 110;
         Ramp_Cnt                       = 0;
 //
 //        //****************** 圆环参数 ********************************************
         ring_cam.Left_Ring_Clc = 1;                 //圆环次数
         ring_cam.Right_Ring_Clc = 1;
         ring_cam.Left_Ring_Type = 0;                //圆环类型（0：50， 1：80）
         ring_cam.Right_Ring_Type = 0;
         ring_cam.StartTurn_Line[0] = 40;
         ring_cam.StartTurn_Line[1] = 36;
         ring_cam.Turn_Upslope[0] = 2;
         ring_cam.Turn_Upslope[1] = 1.4;
         ring_cam.Out_Slope[0] = 2.05;
         ring_cam.Out_Slope[1] = 0.9;
         ring_cam.Out_Angle[0] = 120;          //出环固定打角角度
         ring_cam.Out_Angle[1] = 80;

         /**************************************库参数***********************************************/
         Garage_Out_Angle_L             = 50;//55
         Garage_Out_Angle_R             = 58;//55
         Garage_Out_Imu_Angle           = 60;//66
         Garage_Out_Time                = 180;
         Garage_Out_Inner_Speed         = 20;
         Garage_Out_Outer_Speed         = 70;
         Garage_Out_Mid_Speed           = 40;
         Garage_Clc                     = 1;
         Garage_Direction               = 1;
         Garage_Out                     = 1;

         /*************************************其他参数***********************************************/
         ring_cam.Left_Ring_Clc         = 1;
         ring_cam.Right_Ring_Clc        = 1;
         longStraightAccelerate         = 1;
         longStr_Acc_Speed_1            = 10;
         longStr_Acc_Speed_2            = 30;
         GyroOffset.z                   = 2.17;
         GyroOffset.x                   = 5.23;


         //******************* 路障参数  *************************************
         Barr_Cnt                       = 0;
         Barr_Direction                 = 0;//右
         Barr_Dis_up                    = 1000;
         barricade_Line                 = 12;
         Barr_Fixd_Dis_up               = 26;


         //******************* 断路参数  *************************************
         dg_state.L_max                = 3619;
         dg_state.LM_max               = 3624;
         dg_state.RM_max               = 3621;
         dg_state.R_max                = 3613;



        param_save(0);
        gpio_set_level(LED1, GPIO_HIGH);

        return 1;
    }
    return 1;

}
