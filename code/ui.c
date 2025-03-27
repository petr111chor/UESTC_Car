/*
 * ui.c
 *
 *  Created on: 2023年4月28日
 *      Author: Kern
 */

#include "zf_common_headfile.h"

#define Char_num   21  //简单字符个数宏定义


uint8 main_window_flag = 1;
uint8 child_window_1_flag = 1;

/*10项一级菜单、10项二级菜单*/
float UI_Param[10][10]={0};

/*菜单界面，简单二级菜单*/
char main_table[][Char_num] =
{
    {"Main Menu"},
    {" Setting     "},
    {" Motor       "},
    {" Servo_adc   "},
    {" Image       "},
    {" Garage      "},
    {" Barricade   "},
    {" Break       "},
    {" Ring        "},
    {" Ramp        "},
};
char Second_table[][12][Char_num]={
        {//Setting
         {"Setting"},
         {" is_Garage    "},
         {" Garage_Dir   "},
         {" L_Ring_Clc   "},
         {" R_Ring_Clc   "},
         {" L_Ring_Type  "},
         {" R_Ring_Type  "},
         {" Bar_and_Break"},
         {" Ramp_Cnt     "},
         {" Bar_Cnt      "},
         {" Garage_Clc   "}
        },
        {//电机
         {"Motor"},
         {" Speed        "},
         {" Dir_C_P      "},
         {" Dir_C_D      "},
         {" Adc_Speed    "},
         {" Dir_A_P      "},
         {" Dir_A_D      "},
         {" Motor_P      "},
         {" Motor_I      "},
         {" Dece_K       "},
         {" Begin_time   "},
        },
        {//舵机
         {"Servo "},
         {" Camera_P     "},
         {" Camera_D     "},
         {" ADC_P        "},
         {" ADC_D        "},
         {" ADC_P_k      "},
         {" ADC_thr      "},
         {" All_use_adc  "},
         {" is_ele_stop  "},
         {"              "},
         {"              "}
        },
        {//图像处理
         {"Image"},
         {" Aim_Line     "},
         {" 50R_aim      "},
         {" 80R_aim      "},
         {" is_fix_thr   "},
         {" break_fix_t  "},
         {" threshold    "},
         {" is_OSTU_2    "},
         {" EXP_TIME     "},
         {"              "},
         {"              "}
        },

        {//库
         {"Garage"},
         {" O_Angle_L    "},
         {" O_Angle_R    "},
         {" O_Imu_Angle  "},
         {" O_Time       "},
         {" O_I_Speed    "},
         {" O_O_Speed    "},
         {" O_Mid_Speed  "},
         {" O_Time_Thr   "},
         {"              "}
        },
        {
         {"Barricade"},
         {" Bar_Dir      "},
         {" Bar_Dis      "},
         {" Bar_line     "},
         {" Bar_width1   "},
         {" Bar_width2   "},
         {" Bar_bk_imu   "},
         {"              "},
         {"              "},
         {"              "},
         {"              "}
        },
        {
         {"Break"},
         {" break_line "},
         {" break_time "},
         {" break_rate "},
         {" A          "},
         {" B          "},
         {" C          "},
         {" L_max      "},
         {" LM_max     "},
         {" RM_max     "},
         {" R_max      "}
        },
        {//圆环
         {"Ring"},
         {" S_Turn_50    "},
         {" Turn_k_50    "},
         {" Out_k_50     "},
         {" Out_a_50     "},
         {" S_Turn_80    "},
         {" Turn_k_80    "},
         {" Out_k_80     "},
         {" Out_a_80     "},
         {"              "},
         {"              "}
        },
        {//坡道
         {"Ramp"},
         {" Ramp_dis     "},
         {" Ramp_Aim     "},
         {" Ramp_speed   "},
         {" Ramp_pitch_1 "},
         {" Ramp_pitch_2 "},
         {" Ramp_pitch_3 "},
         {" is_acc       "},
         {" acc_speed_1  "},
         {" acc_speed_2  "},
         {"              "}
        },

};


void select_row(int row)
{

        static int last_row = 1;
        int row_height = 20;  // 每行间距，防止贴在一起
        int x_offset = 72;    // x 坐标偏移，保持图标位置一致

        // 清除上一个选择的行（显示空白图标）
        ips200_show_binary_image(x_offset, last_row * row_height, (const uint8 *)gImage_space, 8, 8, 8, 8);

        // 在新行上绘制选中图标
        ips200_show_binary_image(x_offset, row * row_height, (const uint8 *)gImage_flag, 8, 8, 8, 8);

        // 显示当前选中的行数，确保它不会被图标覆盖
        ips200_show_int(10, 10, row, 1);

        // 记录当前行
        last_row = row;


    /*
    static int last_row = 1;

    ips200_show_binary_image(72, last_row, (const uint8 *)gImage_space, 8,8,8,8);
    ips200_show_binary_image(72, row, (const uint8 *)gImage_flag, 8,8,8,8);
    ips200_show_int(0, 0, row, 1);
    last_row = row;
    */
}



/*****************************************************************
*Function: 显示主菜单
*Description: 显示主菜单（即一级菜单）界面，为了滚动显示效果，需要设定从
*Description: 主菜单列表的哪一行开始显示
*Return:
*****************************************************************/
void show_main_window(uint8 line)
{

       uint16 x = 10, y = 20;  // 初始显示位置

       for(int i = 0; i < sizeof(main_table)/sizeof(main_table[0]); i++)
       {
           ips200_show_string(x, y, main_table[i]);
           y += 20;  // 每行间隔 20 像素，避免字符重叠
       }


    /*oled_show_string(0, 0, (const char*)main_table[0]);
    for(uint8 i=0;i<7;i++)
    {
        ips200_show_string(0,i+1,(const char*)main_table[1+i+line]);
    }
    */
}

/*****************************************************************
*Function: 显示副菜单
*Description: 显示副菜单（即二级菜单）界面
*parameter:  List 哪一个二级菜单，line 从二级菜单的哪一行开始显示
*Return:
*****************************************************************/
void show_second(uint8_t List,uint8_t line)
{

        uint16 x = 10, y = 20;  // 调整起始坐标，避免过于靠近左上角

        // 显示主标题
        ips200_show_string(x, y, (char*)Second_table[List][0]);
        y += 30;  // 标题和列表之间留出间隔

        // 显示菜单内容
        for (uint8 i = 0; i < 7; i++)
        {
            ips200_show_string(x, y, (char*)Second_table[List][1 + i + line]);  // 文本
            ips200_show_float(x + 100, y, UI_Param[List][i + line], 4, 2);      // 数值参数
            y += 20;  // 每行间隔 20 像素，防止重叠
        }


    /*
    ips200_show_string(0, 0, (char*)Second_table[List][0]);
    for(uint8 i=0;i<7;i++)
    {
        ips200_show_string(0,i+1,(char*)Second_table[List][1+i+line]);
        ips200_show_float(80, i+1, UI_Param[List][i+line], 4, 2);
    }
    */
}


/*****************************************************************
*Function: 一级菜单下显示被选中行
*Description: 反转显示被选中的那一行
*parameter: key_val 输入的按键值
*Return:
*****************************************************************/
uint8_t cnt = 0;  // 记录当前窗口起始行
int8_t show_change(key_state_enum key_val)
{
    static int8_t LINE = 1;  // 记录当前选中行
    int8_t MAX_LINE = 8;     // 最大行数（假设菜单最多 8 行）

    // 处理按键输入
    if (key_get_state(KEY_2) == KEY_SHORT_PRESS && LINE > 1)
    {
        LINE--;  // 上移
    }
    else if (key_get_state(KEY_3) == KEY_SHORT_PRESS && LINE < MAX_LINE)
    {
        LINE++;  // 下移
    }

    // 处理菜单翻页逻辑
    if (LINE - cnt >= 8)  // 滚动窗口向下翻页
    {
        cnt++;
        show_main_window(cnt);
    }
    else if (LINE - cnt < 0)  // 滚动窗口向上翻页
    {
        cnt--;
        show_main_window(cnt);
    }

    // 更新选中行的显示
    select_row(LINE - cnt);

    return LINE;
}
/*
uint8_t cnt=0;
int8_t show_change(key_state_enum key_val)
{
    static int8_t LINE=1;

     if(key_get_state(KEY_2) == KEY_SHORT_PRESS && LINE!=0)
         LINE--;
     else if(key_get_state(KEY_3) == KEY_SHORT_PRESS)
         LINE++;

     if(LINE-cnt==8)
     {
          if(cnt==2)
          {
              cnt=0;
              LINE=1;
          }
          else
              cnt++;
          show_main_window(cnt);
     }
     else if(LINE-cnt==0)
     {
        if(cnt==0)
        {
            cnt=2;
            LINE=8;
        }
        else
            cnt--;
        show_main_window(cnt);
     }
     select_row(LINE-cnt);

     return LINE;

}
*/
/*****************************************************************
*Function: 二级菜单下显示被选中行
*Description: 反转显示被选中的那一行
*parameter: key_val 输入的按键值 line 哪个二级菜单 rate 显示调节步长
*Return:
*****************************************************************/
static uint8_t cnt2 = 0;  // 记录二级菜单的起始行
uint8_t show_change2(uint8_t key_val, uint8_t line, float rate)
{
    static uint8_t LINE = 1;       // 记录当前选中的行
    static uint8_t line_last = 0;  // 记录上次的菜单，避免重复显示同一菜单

    // 如果当前菜单与上次菜单不同，重新从头开始显示
    if (line_last != line)
    {
        LINE = 1;  // 重置为第一行
        cnt2 = 0;  // 重置菜单偏移
    }
    line_last = line;

    // 处理按键输入
    if (key_get_state(KEY_2) == KEY_SHORT_PRESS && LINE > 1)
    {
        LINE--;  // 上移
    }
    else if (key_get_state(KEY_3) == KEY_SHORT_PRESS && LINE < 10)  // 假设最大行数为10
    {
        LINE++;  // 下移
    }

    // 处理菜单翻页
    if (LINE - cnt2 >= 8)  // 当前行超出菜单范围，向下翻页
    {
        cnt2++;
        show_second(line, cnt2);
    }
    else if (LINE - cnt2 < 0)  // 当前行超出菜单范围，向上翻页
    {
        cnt2--;
        show_second(line, cnt2);
    }

    // 处理长按按键调节
    if (key_get_state(KEY_3) == KEY_LONG_PRESS)
    {
        UI_Param[line][LINE - 1] += rate;  // 增加值
    }
    else if (key_get_state(KEY_2) == KEY_LONG_PRESS)
    {
        UI_Param[line][LINE - 1] -= rate;  // 减少值
    }

    // 更新选中行的显示
    select_row(LINE - cnt2);

    // 显示当前调节的步长和参数值
    ips200_show_float(90, 0, rate, 4, 2);
    ips200_show_float(80, LINE - cnt2, UI_Param[line][LINE - 1], 4, 2);

    key_clear_all_state();  // 清除按键状态
    key_scanner();  // 扫描按键

    return LINE;  // 返回当前选中的行
}
/*
static uint8_t cnt2=0;
uint8_t show_change2(uint8_t key_val,uint8_t line,float rate)
{
    static uint8_t LINE=1;
    static uint8_t line_last=0;


    if(line_last!=line)//判断两次是否是同一个二级菜单
    {
        LINE = 1;//若不是则从头显示
        cnt2 = 0;
    }
    line_last = line;

     if(key_get_state(KEY_2) == KEY_SHORT_PRESS&&LINE!=0)
         LINE--;
     else if(key_get_state(KEY_3) == KEY_SHORT_PRESS)
         LINE++;


     if(LINE-cnt2==8)
     {
          if(cnt2==3)
          {
              cnt2=0;//翻到底了
              LINE=1;
          }
          else
              cnt2++;
          show_second(line,cnt2);
     }
     else if(LINE-cnt2==0)
     {
        if(cnt2==0)
        {
            cnt2=3;//翻到顶了
            LINE=10;
        }
        else
            cnt2--;

        show_second(line,cnt2);
     }
     key_clear_all_state();
     key_scanner();
     if(key_get_state(KEY_3) == KEY_LONG_PRESS)
         UI_Param[line][LINE-1]+=rate;
     else if (key_get_state(KEY_2) == KEY_LONG_PRESS)
         UI_Param[line][LINE-1]-=rate;

     select_row(LINE-cnt2);

     ips200_show_float(90, 0, rate, 4, 2);
     ips200_show_float(80, LINE-cnt2, UI_Param[line][LINE-1], 4, 2);


 return LINE;

}
*/
/*****************************************************************
*Function: 步长速率转换
*Description:
*parameter: cnt 1-5，共六个步长单位
*Return:
*****************************************************************/
float step_rate(uint8_t cnt)
{
    if(cnt%6==1)      return 1.0f;
    else if(cnt%6==2) return 10.0f;
    else if(cnt%6==3) return 100.0f;
    else if(cnt%6==4) return 1000.0f;
    else if(cnt%6==5) return 0.010f;
    else              return 0.1f;

}


void show_ui(void)
{
    short res_1 = 1;
    key_clear_all_state();
    uint16_t wave_flag=0;
    int16_t LINE_1=1;
    static uint16_t flag_exit = 0;



    // ********* 显示封面 ******************
    ips200_show_rgb565_image(0, 120, (const uint16 *)gImage_seekfree_logo, 240, 80, 240, 80, 0);
    system_delay_ms(1000);
    ips200_clear();
    ips200_show_string(0, 0, "UESTC");
    system_delay_ms(1000);
    flash_read_page(SECTION_INDEX, PAGE_INDEX, flash_data_buffer, DATA_SIZE);

    /*while(res_1)//原来是res_1
    {
        res_1 = Data_init();
        system_delay_ms(200);
    }
*/
    param_read();

/*
    Garage_Direction               = (uint8_t)(flash_data_buffer[1] / 1000);
    Garage_Out                     = (uint8_t)(flash_data_buffer[0] / 1000);
    Garage_Out_Angle_L             = (int16_t)(flash_data_buffer[40] / 1000);
    Garage_Out_Angle_R             = (int16_t)(flash_data_buffer[41] / 1000);

    if(Garage_Out > 0)
    {
        if(Garage_Direction == 1)
            ServoCtrl(Servo_Center_Mid - Garage_Out_Angle_R);
        else {
            ServoCtrl(Servo_Center_Mid + Garage_Out_Angle_L);
        }
    }
    else{
        CHARGE_ON;
    }
*///前车舵机删除
    // 开始 UI 调参
    ips200_clear();

    show_main_window(0);

    select_row(1);

    while(1)
    {
        //key = key_Scan();
        key_clear_all_state();
        key_scanner();
        // 选择一级菜单
        if(wave_flag==0)
        {
          if(key_get_state(KEY_2) == KEY_SHORT_PRESS||key_get_state(KEY_3) == KEY_SHORT_PRESS)
              LINE_1 = show_change(KEY_2);//一级菜单选择
        }


        else if(wave_flag>0)
        {
            key_clear_all_state();
            key_scanner();
            if(key_get_state(KEY_2) == KEY_SHORT_PRESS||key_get_state(KEY_3) == KEY_SHORT_PRESS||key_get_state(KEY_2) == KEY_LONG_PRESS||key_get_state(KEY_3) == KEY_LONG_PRESS)
              show_change2(KEY_2, LINE_1-1,step_rate(wave_flag));
        }

        key_clear_all_state();
        key_scanner();
        if(key_get_state(KEY_1) == KEY_SHORT_PRESS)//进入
        {
            flag_exit = 0;
            wave_flag++;
            if(wave_flag==1)
            {
                ips200_clear();//清屏
                show_second(LINE_1-1, 0);
            }
            show_change2(KEY_1, LINE_1-1,step_rate(wave_flag));
        }

        else if(wave_flag!=0&&key_get_state(KEY_1) == KEY_SHORT_PRESS)
        {
                key_clear_all_state();
                flag_exit ++;
                if(flag_exit<3){
                wave_flag=0;
                ips200_clear();

                show_main_window(cnt);
                show_change(KEY_1);
                param_save(1);

                // ************** 参数初始化 ************************************
                Param_Init();
            }
        }

        if(wave_flag==0 && (key_get_state(KEY_1) == KEY_SHORT_PRESS||!Flag.Duty_Stop_Flag))
        {
            ips200_clear();
            Flag.Ui_Stop_Flag = 0;
            Flag.Duty_Stop_Flag = 0;
            break;
        }

        system_delay_ms(100);
    }

}







