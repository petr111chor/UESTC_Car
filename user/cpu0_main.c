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
            佛祖保佑       永无BUG
*/
#include "zf_common_headfile.h"
char txt[32];
char tele_num = 0;



//void Iwdg_init(void)
//{
    // 独立看门狗，防止程序跑飞
//    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
//    IWDG_SetPrescaler(IWDG_Prescaler_4);
//    IWDG_SetReload(3000);
//    IWDG_ReloadCounter();
//    IWDG_Enable();
//}

void Iwdg_init(void)
{
    // 获取当前CPU核心的看门狗实例
    Ifx_SCU_WDTCPU *wdt = &MODULE_SCU.WDTCPU[IfxCpu_getCoreIndex()];

    // 初始化看门狗配置结构体
    IfxScuWdt_Config wdtConfig;
    IfxScuWdt_initConfig(&wdtConfig); // 加载默认配置

    // 自定义配置参数
    wdtConfig.password           = 0x3C;     // 密码（保持默认0x3C）
    wdtConfig.reload             = 3000;     // 重载值
    wdtConfig.inputFrequency     = IfxScu_WDTCON1_IR_divBy64; // 分频系数64
    wdtConfig.disableWatchdog    = FALSE;    // 使能看门狗

    // 初始化CPU看门狗
    IfxScuWdt_initCpuWatchdog(wdt, &wdtConfig);
}


int core0_main(void)
{
    clock_init(); // 初始化芯片时钟
    debug_init();                  // 初始化默认 Debug UART
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

    // ************* 中断 初始化 ********************************
    //pit_ms_init(CCU60_CH0, 10); // 总中断
    pit_ms_init(CCU60_CH1, 5);
   // ************* 看门狗 初始化 ********************************

    cpu_wait_event_ready();
    //MotorCtrl(-2000,-2000);
    while(1)
    {
        IfxScuWdt_serviceCpuWatchdog(0x3C); // 使用相同的密码喂狗
        key_clear_all_state();
        key_scanner();
        if(key_get_state(KEY_1) == KEY_SHORT_PRESS)
            pd_flag = pd_flag==1?0:1;                                            // 翻转 LED 引脚输出电平 控制 LED 亮灭

        if(key_get_state(KEY_2) == KEY_SHORT_PRESS)
                    //threshold -= 5;
            if(pd_flag)
            motor_pid_direction.kp+=0.1;
            else
                motor_pid_direction.kd+=0.1;

        if(key_get_state(KEY_3) == KEY_SHORT_PRESS)
            if(pd_flag)
                motor_pid_direction.kp-=0.1;
            else
                motor_pid_direction.kd-=0.1;
        // 翻转 LED 引脚输出电平 控制 LED 亮灭

        if(key_get_state(KEY_4) == KEY_SHORT_PRESS)
                    break;
        ips200_show_string(25, 200, "kp:"); ips200_show_float(25, 225, motor_pid_direction.kp, 3,3);
        ips200_show_string(110, 200, "kd:");ips200_show_float(110, 225, motor_pid_direction.kd, 3,3);
        ips200_show_int(110,240,motor_param.Ui_Speed,3);
    }
    while (1)
    {
        // 喂狗

        IfxScuWdt_serviceCpuWatchdog(0x3C); // 使用相同的密码喂狗
        //wifi(32);


        if (mt9v03x_finish_flag)//mt9v03x_finish_flag
                {
                    //oled_show_int(0, 0, Total_time, 3);
                    //garage_out_process(); //出库
                    /******************图像处理*******************************/
                    show_img_flag = 0;
                    ImageProcess();




                    //mt9v03x_test();
                    Get_White_Num();
                    //trackDFS();
                    Left_Right_Confusion_Filter();

                    TurnPointL_Find(&LTurnPoint_Row, &LTurnPoint, &Kl_up, &Bl_up, &Kl_down, &Bl_down, &Error_Cnt_L);
                    TurnPointR_Find(&RTurnPoint_Row, &RTurnPoint, &Kr_up, &Br_up, &Kr_down, &Br_down, &Error_Cnt_R);

                    doFilter();
                    doMend();
                   // Break_Road_2();
                    updateMediumLine();


                    show_img_flag = 1;
                    mt9v03x_finish_flag = 0;

                }

    }
    return 0;

}
// **************************** test **********************************************
// 启动测试程序需要把主程序入口注释
//      led_test();               // 测试   led  程序
//      oled_test();              // 测试   oled 程序
//      wifi_test();              // 测试   wifi 程序
//      motor_test();             // 测试   电机     程序
//      servo_test();             // 测试   舵机     程序
//      beep_test();              // 测试   蜂鸣器  程序
//      wireless_test();          // 测试   无线      程序
//      mt9v03x_test();           // 测试   摄像头  程序
//      encoder_dir_test();       // 测试   方向编码器  程序
//      infrared_test();          // 测试    红外遥控 程序
//      dl1a_test();              // 测试    超声波   程序
//      ele_mad_test();           // 测试    电磁      程序
//      blue_test();              // 测试    蓝牙       程序
//      encoder_test();           // 测试   编码器  程序
// **************************** test **********************************************
