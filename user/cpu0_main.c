/*
                   _ooOoo_
                  o8888888o
                  88" . "88
                  (| -_- |)
                  O\  =  /O
               ____/`---'\____
             .'  \\|     |//  `.
            /  \\|||  :  |||//  \
           /  _||||| -:- |||||-  \
           |   | \\\  -  /// |   |
           | \_|  ''\---/''  |   |
           \  .-\__  `-`  ___/-. /
         ___`. .'  /--.--\  `. . __
      ."" '<  `.___\_<|>_/___.'  >'"".
     | | :  `- \`.;`\ _ /`;.`/ - ` : | |
     \  \ `-.   \_ __\ /__ _/   .-` /  /
======`-.____`-.___\_____/___.-`____.-'======
                   `=---='
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
            ���汣��       ����BUG
*/
#include "zf_common_headfile.h"
char txt[32];
char tele_num = 0;



//void Iwdg_init(void)
//{
    // �������Ź�����ֹ�����ܷ�
//    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
//    IWDG_SetPrescaler(IWDG_Prescaler_4);
//    IWDG_SetReload(3000);
//    IWDG_ReloadCounter();
//    IWDG_Enable();
//}

void Iwdg_init(void)
{
    // ��ȡ��ǰCPU���ĵĿ��Ź�ʵ��
    Ifx_SCU_WDTCPU *wdt = &MODULE_SCU.WDTCPU[IfxCpu_getCoreIndex()];

    // ��ʼ�����Ź����ýṹ��
    IfxScuWdt_Config wdtConfig;
    IfxScuWdt_initConfig(&wdtConfig); // ����Ĭ������

    // �Զ������ò���
    wdtConfig.password           = 0x3C;     // ���루����Ĭ��0x3C��
    wdtConfig.reload             = 3000;     // ����ֵ
    wdtConfig.inputFrequency     = IfxScu_WDTCON1_IR_divBy64; // ��Ƶϵ��64
    wdtConfig.disableWatchdog    = FALSE;    // ʹ�ܿ��Ź�

    // ��ʼ��CPU���Ź�
    IfxScuWdt_initCpuWatchdog(wdt, &wdtConfig);
}


int core0_main(void)
{
    clock_init(); // ��ʼ��оƬʱ��
    debug_init();                  // ��ʼ��Ĭ�� Debug UART
    all_init();
    MotorInit(2000);
    Encoder_Init();
    key_init(5);
    ips200_init(IPS200_TYPE_SPI);

//    CHARGE_ON;

//    show_ui();
//    mt9v03x_test();

    Param_Init();
    threshold = 230;
    light = 250;
    mt9v03x_set_exposure_time(light);

    //servo_test();

    //MotorCtrl(2000, 2000);
    system_delay_ms(1000);
    ips200_clear();

    // ************* �ж� ��ʼ�� ********************************
    //pit_ms_init(CCU60_CH0, 10); // ���ж�
    pit_ms_init(CCU60_CH1, 5);
   // ************* ���Ź� ��ʼ�� ********************************

    cpu_wait_event_ready();
    //MotorCtrl(-2000,-2000);
    while (1)
    {
        // ι��

        IfxScuWdt_serviceCpuWatchdog(0x3C); // ʹ����ͬ������ι��
        //wifi(32);

        if (mt9v03x_finish_flag)//mt9v03x_finish_flag
                {
                    //oled_show_int(0, 0, Total_time, 3);
                    //garage_out_process(); //����
                    /******************ͼ����*******************************/
                    ImageProcess();
                    ips200_show_gray_image(25, 50, (const uint8 *)C_Image, ImageW, ImageH, ImageW*2, ImageH*2, threshold);
                    ips200_show_int(25, 225, threshold, 3);
                    ips200_show_string(25, 200, "threshold:");
                    ips200_show_int(110, 225, light, 3);
                    ips200_show_string(110, 200, "light:");

                    key_clear_all_state();
                    key_scanner();
                    if(key_get_state(KEY_1) == KEY_SHORT_PRESS)
                                threshold += 5;                                            // ��ת LED ���������ƽ ���� LED ����

                    if(key_get_state(KEY_2) == KEY_SHORT_PRESS)
                                threshold -= 5;

                    if(key_get_state(KEY_3) == KEY_SHORT_PRESS)
                                light += 50;                                            // ��ת LED ���������ƽ ���� LED ����

                    if(key_get_state(KEY_4) == KEY_SHORT_PRESS)
                                light -= 50;

                    //mt9v03x_test();
                    Get_White_Num();
                    trackDFS();
                    Left_Right_Confusion_Filter();
                    if (!Flag.Ramp && !Flag.barricade && !Flag.Garage_Out && !Flag.break_Road)
                    {
                        TurnPointL_Find(&LTurnPoint_Row, &LTurnPoint, &Kl_up, &Bl_up, &Kl_down, &Bl_down, &Error_Cnt_L);
                        TurnPointR_Find(&RTurnPoint_Row, &RTurnPoint, &Kr_up, &Br_up, &Kr_down, &Br_down, &Error_Cnt_R);
                        zebra_found_zz();
                        Left_Ring();
                        Right_Ring();
                    }

                    doFilter();
                    doMend();
                    barricade_by_image();
                    Ramp_task();
                   // Break_Road_2();
                    updateMediumLine();
                    show_line();
                    //turn_Update(); //��������Ƕ�
                    ips200_show_int(150,230,servo_param.Servo_filter,3);



                    mt9v03x_finish_flag = 0;

                }
        ips200_show_string(10,240,"turn_value"); ips200_show_int(110,240,ssss,3);
        ips200_show_string(10,260,"l_speed"); ips200_show_int(110,260,motor_l.encoder_speed,3);
        ips200_show_string(10,280,"r_speed"); ips200_show_int(110,280,motor_r.encoder_speed,3);
        ips200_show_string(10,300,"difference"); ips200_show_int(110,300,motor_l.encoder_speed-motor_r.encoder_speed,3);


        //system_delay(1000);
        printf("%d,%d,%d,%d\n",motor_l.encoder_speed,motor_r.encoder_speed,motor_l.target_speed,motor_r.target_speed);

    }
    return 0;

}
// **************************** test **********************************************
// �������Գ�����Ҫ�����������ע��
//      led_test();               // ����   led  ����
//      oled_test();              // ����   oled ����
//      wifi_test();              // ����   wifi ����
//      motor_test();             // ����   ���     ����
//      servo_test();             // ����   ���     ����
//      beep_test();              // ����   ������  ����
//      wireless_test();          // ����   ����      ����
//      mt9v03x_test();           // ����   ����ͷ  ����
//      encoder_dir_test();       // ����   ���������  ����
//      infrared_test();          // ����    ����ң�� ����
//      dl1a_test();              // ����    ������   ����
//      ele_mad_test();           // ����    ���      ����
//      blue_test();              // ����    ����       ����
//      encoder_test();           // ����   ������  ����
// **************************** test **********************************************
