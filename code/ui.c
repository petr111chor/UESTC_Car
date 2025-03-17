/*
 * ui.c
 *
 *  Created on: 2023��4��28��
 *      Author: Kern
 */

#include "zf_common_headfile.h"

#define Char_num   21  //���ַ������궨��


uint8 main_window_flag = 1;
uint8 child_window_1_flag = 1;

/*10��һ���˵���10������˵�*/
float UI_Param[10][10]={0};

/*�˵����棬�򵥶����˵�*/
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
        {//���
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
        {//���
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
        {//ͼ����
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

        {//��
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
        {//Բ��
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
        {//�µ�
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
        int row_height = 20;  // ÿ�м�࣬��ֹ����һ��
        int x_offset = 72;    // x ����ƫ�ƣ�����ͼ��λ��һ��

        // �����һ��ѡ����У���ʾ�հ�ͼ�꣩
        ips200_show_binary_image(x_offset, last_row * row_height, (const uint8 *)gImage_space, 8, 8, 8, 8);

        // �������ϻ���ѡ��ͼ��
        ips200_show_binary_image(x_offset, row * row_height, (const uint8 *)gImage_flag, 8, 8, 8, 8);

        // ��ʾ��ǰѡ�е�������ȷ�������ᱻͼ�긲��
        ips200_show_int(10, 10, row, 1);

        // ��¼��ǰ��
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
*Function: ��ʾ���˵�
*Description: ��ʾ���˵�����һ���˵������棬Ϊ�˹�����ʾЧ������Ҫ�趨��
*Description: ���˵��б����һ�п�ʼ��ʾ
*Return:
*****************************************************************/
void show_main_window(uint8 line)
{

       uint16 x = 10, y = 20;  // ��ʼ��ʾλ��

       for(int i = 0; i < sizeof(main_table)/sizeof(main_table[0]); i++)
       {
           ips200_show_string(x, y, main_table[i]);
           y += 20;  // ÿ�м�� 20 ���أ������ַ��ص�
       }


    /*oled_show_string(0, 0, (const char*)main_table[0]);
    for(uint8 i=0;i<7;i++)
    {
        ips200_show_string(0,i+1,(const char*)main_table[1+i+line]);
    }
    */
}

/*****************************************************************
*Function: ��ʾ���˵�
*Description: ��ʾ���˵����������˵�������
*parameter:  List ��һ�������˵���line �Ӷ����˵�����һ�п�ʼ��ʾ
*Return:
*****************************************************************/
void show_second(uint8_t List,uint8_t line)
{

        uint16 x = 10, y = 20;  // ������ʼ���꣬������ڿ������Ͻ�

        // ��ʾ������
        ips200_show_string(x, y, (char*)Second_table[List][0]);
        y += 30;  // ������б�֮���������

        // ��ʾ�˵�����
        for (uint8 i = 0; i < 7; i++)
        {
            ips200_show_string(x, y, (char*)Second_table[List][1 + i + line]);  // �ı�
            ips200_show_float(x + 100, y, UI_Param[List][i + line], 4, 2);      // ��ֵ����
            y += 20;  // ÿ�м�� 20 ���أ���ֹ�ص�
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
*Function: һ���˵�����ʾ��ѡ����
*Description: ��ת��ʾ��ѡ�е���һ��
*parameter: key_val ����İ���ֵ
*Return:
*****************************************************************/
uint8_t cnt = 0;  // ��¼��ǰ������ʼ��
int8_t show_change(key_state_enum key_val)
{
    static int8_t LINE = 1;  // ��¼��ǰѡ����
    int8_t MAX_LINE = 8;     // �������������˵���� 8 �У�

    // ����������
    if (key_get_state(KEY_2) == KEY_SHORT_PRESS && LINE > 1)
    {
        LINE--;  // ����
    }
    else if (key_get_state(KEY_3) == KEY_SHORT_PRESS && LINE < MAX_LINE)
    {
        LINE++;  // ����
    }

    // ����˵���ҳ�߼�
    if (LINE - cnt >= 8)  // �����������·�ҳ
    {
        cnt++;
        show_main_window(cnt);
    }
    else if (LINE - cnt < 0)  // �����������Ϸ�ҳ
    {
        cnt--;
        show_main_window(cnt);
    }

    // ����ѡ���е���ʾ
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
*Function: �����˵�����ʾ��ѡ����
*Description: ��ת��ʾ��ѡ�е���һ��
*parameter: key_val ����İ���ֵ line �ĸ������˵� rate ��ʾ���ڲ���
*Return:
*****************************************************************/
static uint8_t cnt2 = 0;  // ��¼�����˵�����ʼ��
uint8_t show_change2(uint8_t key_val, uint8_t line, float rate)
{
    static uint8_t LINE = 1;       // ��¼��ǰѡ�е���
    static uint8_t line_last = 0;  // ��¼�ϴεĲ˵��������ظ���ʾͬһ�˵�

    // �����ǰ�˵����ϴβ˵���ͬ�����´�ͷ��ʼ��ʾ
    if (line_last != line)
    {
        LINE = 1;  // ����Ϊ��һ��
        cnt2 = 0;  // ���ò˵�ƫ��
    }
    line_last = line;

    // ����������
    if (key_get_state(KEY_2) == KEY_SHORT_PRESS && LINE > 1)
    {
        LINE--;  // ����
    }
    else if (key_get_state(KEY_3) == KEY_SHORT_PRESS && LINE < 10)  // �����������Ϊ10
    {
        LINE++;  // ����
    }

    // ����˵���ҳ
    if (LINE - cnt2 >= 8)  // ��ǰ�г����˵���Χ�����·�ҳ
    {
        cnt2++;
        show_second(line, cnt2);
    }
    else if (LINE - cnt2 < 0)  // ��ǰ�г����˵���Χ�����Ϸ�ҳ
    {
        cnt2--;
        show_second(line, cnt2);
    }

    // ��������������
    if (key_get_state(KEY_3) == KEY_LONG_PRESS)
    {
        UI_Param[line][LINE - 1] += rate;  // ����ֵ
    }
    else if (key_get_state(KEY_2) == KEY_LONG_PRESS)
    {
        UI_Param[line][LINE - 1] -= rate;  // ����ֵ
    }

    // ����ѡ���е���ʾ
    select_row(LINE - cnt2);

    // ��ʾ��ǰ���ڵĲ����Ͳ���ֵ
    ips200_show_float(90, 0, rate, 4, 2);
    ips200_show_float(80, LINE - cnt2, UI_Param[line][LINE - 1], 4, 2);

    key_clear_all_state();  // �������״̬
    key_scanner();  // ɨ�谴��

    return LINE;  // ���ص�ǰѡ�е���
}
/*
static uint8_t cnt2=0;
uint8_t show_change2(uint8_t key_val,uint8_t line,float rate)
{
    static uint8_t LINE=1;
    static uint8_t line_last=0;


    if(line_last!=line)//�ж������Ƿ���ͬһ�������˵�
    {
        LINE = 1;//���������ͷ��ʾ
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
              cnt2=0;//��������
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
            cnt2=3;//��������
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
*Function: ��������ת��
*Description:
*parameter: cnt 1-5��������������λ
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



    // ********* ��ʾ���� ******************
    ips200_show_rgb565_image(0, 120, (const uint16 *)gImage_seekfree_logo, 240, 80, 240, 80, 0);
    system_delay_ms(1000);
    ips200_clear();
    ips200_show_string(0, 0, "UESTC");
    system_delay_ms(1000);
    flash_read_page(SECTION_INDEX, PAGE_INDEX, flash_data_buffer, DATA_SIZE);

    /*while(res_1)//ԭ����res_1
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
*///ǰ�����ɾ��
    // ��ʼ UI ����
    ips200_clear();

    show_main_window(0);

    select_row(1);

    while(1)
    {
        //key = key_Scan();
        key_clear_all_state();
        key_scanner();
        // ѡ��һ���˵�
        if(wave_flag==0)
        {
          if(key_get_state(KEY_2) == KEY_SHORT_PRESS||key_get_state(KEY_3) == KEY_SHORT_PRESS)
              LINE_1 = show_change(KEY_2);//һ���˵�ѡ��
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
        if(key_get_state(KEY_1) == KEY_SHORT_PRESS)//����
        {
            flag_exit = 0;
            wave_flag++;
            if(wave_flag==1)
            {
                ips200_clear();//����
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

                // ************** ������ʼ�� ************************************
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







