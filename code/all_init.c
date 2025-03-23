/*
 * all_init.c
 *
 *  Created on: 2023��4��27��
 */


#include "zf_common_headfile.h"

#define USE_BATTERY                        (1)
#define USE_WIRELESS                       (0)
#define USE_WIFI                           (0)
#define USE_INFRARED                       (0)
#define USE_DL1A                           (1)
#define USE_BLUE                           (1)
#define USE_SD                             (0)


void wireless_init(void)
{
    if(wireless_uart_init())                                                    // �ж��Ƿ�ͨ����ʼ��
    {
        while(1)                                                                // ��ʼ��ʧ�ܾ����������ѭ��
        {
            gpio_toggle_level(LED1);                                            // ��ת LED ���������ƽ ���� LED ����
            system_delay_ms(100);                                               // ����ʱ�������Ʊ�ʾ�쳣
        }
    }
}


void wifi_init(void)
{
    while(wifi_uart_init(WIFI_SSID, WIFI_PASSWORD, WIFI_UART_STATION))
    {
        //oled_show_string(OLED_MID, 0, "Init error");
        printf("Init error, try again.\r\n");
        system_delay_ms(100);                                                   // ��ʼ��ʧ�� �ȴ� 500ms
    }
}

void my_camera_init(void)
{
    while(1)
    {
        if(mt9v03x_init())
            ips200_show_string(0, 1, "mt9v03x reinit.");
       else
            break;
        system_delay_ms(100);                                                   // ����ʱ�������Ʊ�ʾ�쳣
   }
}

void encoder_init(void)
{
    encoder_dir_init(TIM2_ENCODER, TIM2_ENCODER_CH1_P00_7, TIM2_ENCODER_CH2_P00_8);
    encoder_dir_init(TIM3_ENCODER, TIM3_ENCODER_CH1_P02_6, TIM3_ENCODER_CH2_P02_7);
}

void infrared_init(void)
{
    gpio_init(INFRARED_PIN_A,GPI,1,GPI_PULL_DOWN);
    gpio_init(INFRARED_PIN_B,GPI,1,GPI_PULL_DOWN);
    exti_init(INFRARED_PIN_A, EXTI_TRIGGER_BOTH);
    exti_init(INFRARED_PIN_B, EXTI_TRIGGER_BOTH);
}

void ele_mag_init(void)
{
    adc_init(L_ADC, ADC_12BIT);
    adc_init(LM_ADC, ADC_12BIT);
    adc_init(RM_ADC, ADC_12BIT);
    adc_init(R_ADC, ADC_12BIT);
}

void led_init(void)
{
    gpio_init(LED1, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(LED2, GPO, GPIO_HIGH, GPO_PUSH_PULL);
}

void my_dl1a_init(void)
{
    while(1)
    {
        if(dl1b_init())
        {
            wireless_uart_send_string("DL1A init error.\r\n");
        }
        else
        {
            break;
        }
        gpio_toggle_level(LED1);                                                // ��ת LED ���������ƽ ���� LED ���� ��ʼ����������ƻ����ĺ���
    }
}


void blue_init(void)
{
    if(bluetooth_ch9141_init())                                                 // �ж��Ƿ�ͨ����ʼ��
    {
        while(1)                                                                // ��ʼ��ʧ�ܾ����������ѭ��
        {
            gpio_toggle_level(LED1);                                            // ��ת LED ���������ƽ ���� LED ����
            system_delay_ms(100);                                               // ����ʱ�������Ʊ�ʾ�쳣
        }
    }
}

//void  mv_dm1xa_init(void)
//{
//    while(1)
//    {
//        if(dm1xa_init())
//        {
//        }
//        else
//        {
//            break;
//        }

//    }
//}

void charge_init(void)
{
    gpio_init(CHARGE_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
}


void all_init(void)
{
    // ************** ���������������� ��ʼ�� ********************************
    //beep_init();        // ��ʱ�� 9
    //MotorInit(2000);    // ��ʱ�� 8
    //ServoInit(50);       // ��ʱ�� 5

    // ************** ��������ʼ�� ************************************
    //encoder_init();

    // ************* UI ��ʼ�� ********************************
    key_init(10);
    //oled_init();
    ips200_init(IPS200_TYPE_SPI);
    ips200_show_rgb565_image(0, 120, (const uint16 *)gImage_seekfree_logo, 240, 80, 240, 80, 0);    // ��ʾһ��RGB565ɫ��ͼƬ ԭͼ240*80 ��ʾ240*80 ��λ��ǰ
    system_delay_ms(1500);

    ips200_full(RGB565_PINK);
    system_delay_ms(1000);
    ips200_show_string(0, 0, "UESTC's Fourier");

    // ************* LED��ʼ�� *******************************
    //led_init();

    // ************* ����ʼ�� ***************************************
    //charge_init();

    // ************* �����ǳ�ʼ�� **************************************
    //while(imu963ra_init())

/*

    // ************** ����ң�� ��ʼ�� *************************************
#if USE_INFRARED
    infrared_init();
#endif


#if USE_BATTERY

    // ************** ���ADC ��ʼ�� ************************************��
    ele_mag_init();


#if pc_mono
    // ************* ���ߴ��� ��ʼ�� ********************************
    wireless_init();
#endif

#if USE_WIFI
    // ************* WIFI ��ʼ�� ********************************
    wifi_init();
#endif


#if USE_BLUE
    // ************** ���� ��ʼ�� **************************************
    blue_init();
#endif


*/
    // ************** ����ͷ ��ʼ�� ************************************��
     my_camera_init();
/*
#if USE_DL1A
    // ************** ������ ��ʼ�� *************************************
   my_dl1a_init();
  // mv_dm1xa_init();
#endif


#if  Recoder
    if(!SD_Card_Init())//SD����ʼ��
       {
          SD_CS(1); //�ر�SD��
       }
#endif

#endif
*/
}
