/*
 * module_test.c
 *
 *  Created on: 2023年5月5日
 *      Author: Kern
 */

#include "zf_common_headfile.h"
char txt1[32];

/*-------------------------------------------------------------------------------------------------------------------
// 函数简介     LED测试
* 接线定义：
*                   ------------------------------------
*                   模块管脚             单片机管脚
*                   LED管脚               LED1
*                   ------------------------------------
//-------------------------------------------------------------------------------------------------------------------*/
void led_test(void)
{

    gpio_set_level(LED1, GPIO_LOW);

    while(1)
    {
        if(key_get_state(KEY_1) == KEY_SHORT_PRESS)
            gpio_toggle_level(LED2);                                            // 翻转 LED 引脚输出电平 控制 LED 亮灭


        system_delay_ms(200);

        if(key_get_state(KEY_1) == KEY_RELEASE)
            return;
    }
}

void infer_test(void)
{

    key_init(10);
    key_scanner();

    while(1)
    {
        if(key_get_state(KEY_1) == KEY_SHORT_PRESS)
            threshold += 5;                                            // 翻转 LED 引脚输出电平 控制 LED 亮灭


        system_delay_ms(200);

        if(key_get_state(KEY_2) == KEY_SHORT_PRESS)
            threshold -= 5;
    }
}


/*-------------------------------------------------------------------------------------------------------------------
// 函数简介     ips200测试
* 接线定义：
*                   ------------------------------------
*                   模块管脚             单片机管脚
*                   D0                  查看 zf_device_ips200.h 中 ips200_D0_PIN 宏定义 B13
*                   D1                  查看 zf_device_ips200.h 中 ips200_D1_PIN 宏定义 B15
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   ------------------------------------
//-------------------------------------------------------------------------------------------------------------------*/
void ips200_test (void)
{
      uint16_t data[128];
      uint16_t data_index = 0;
      for( ; data_index < 64; data_index ++)
          data[data_index] = data_index;
      for(data_index = 64; data_index < 128; data_index ++)
          data[data_index] = 128 - data_index;

      ips200_set_dir(IPS200_PORTAIT);
      ips200_set_font(IPS200_6X8_FONT);
      ips200_init(IPS200_TYPE_SPI);
      // 此处编写用户代码 例如外设初始化代码等

      while(1)
      {
          // 此处编写需要循环执行的代码
          ips200_clear();
          ips200_show_string(0, 0, "SEEKFREE");                                     // 显示字符串
          ips200_show_float(0, 1, -13.141592, 1, 6);                                // 显示 float 数据 1bit 整数 6bit 小数 应当显示 -3.141592 总共会有 9 个字符的显示占位
          ips200_show_float(64, 1, 13.141592, 8, 4);                                // 显示 float 数据 8bit 整数 4bit 小数 应当显示 13.1415 总共会有 14 个字符的显示占位 后面会有 5 个字符的空白占位
          ips200_show_int(0, 2, -127, 2);                                           // 显示 int8 数据
          ips200_show_uint(64, 2, 255, 4);                                          // 显示 uint8 数据
          ips200_show_int(64, 3, -32768, 4);                                        // 显示 int16 数据
          ips200_show_uint(0, 3, 65535, 6);                                         // 显示 uint16 数据
          ips200_show_int(0, 4, -2147483648, 8);                                    // 显示 int32 数据 8bit 整数 应当显示 -47483648
          ips200_show_uint(0, 5, 4294967295, 10);                                   // 显示 uint32 数据 10bit 整数 应当显示 4294967295
          //ips200_show_chinese(0, 6, 16, (const uint8 *)IPS200_16x16_chinese, 4);
          system_delay_ms(1000);

          ips200_clear();
          ips200_show_wave(0, 0, data, 128, 64, 64, 32);                            // 显示一个三角波形 波形宽度 128 波形最大值 64 显示宽度 64 显示最大值 32
          system_delay_ms(1000);
          ips200_clear();
          ips200_show_wave(0, 0, data, 128, 64, 128 ,64);                           // 显示一个三角波形 波形宽度 128 波形最大值 64 显示宽度 128 显示最大值 64
          system_delay_ms(1000);

          ips200_clear();
          system_delay_ms(1000);
          ips200_full(0xFF);
          system_delay_ms(1000);
          // 此处编写需要循环执行的代码

          if(key_get_state(KEY_1) == KEY_SHORT_PRESS)
          {
              ips200_clear();
              return;
          }

      }

}




uint8 wifi_uart_test_buffer[] = "this is wifi uart test buffer";
uint8 wifi_uart_get_data_buffer[256];
uint16 data_length;
char *link_id = NULL;
/*-------------------------------------------------------------------------------------------------------------------
// 函数简介     WIFI测试
* 接线定义：
*                   ------------------------------------
*                   模块管脚            单片机管脚
*                   RX                  查看 zf_device_wifi_uart.h 中 WRIELESS_UART_RX_PINx  宏定义  E12
*                   TX                  查看 zf_device_wifi_uart.h 中 WRIELESS_UART_TX_PINx  宏定义  E13
*                   RTS                 查看 zf_device_wifi_uart.h 中 WRIELESS_UART_RTS_PINx 宏定义  E8
*                   RST                 查看 zf_device_wifi_uart.h 中 WRIELESS_UART_RST_PINx 宏定义 E9
*                   VCC                 5V 电源
*                   GND                 电源地
*                   其余引脚悬空
*                   ------------------------------------
-------------------------------------------------------------------------------------------------------------------*/
void wifi_test(void)
{
      ips200_show_string(0, 1, "YES");
      printf("module version:%s\r\n", wifi_uart_information.wifi_uart_version);   // 模块固件版本
      printf("module mac    :%s\r\n", wifi_uart_information.wifi_uart_mac);       // 模块 MAC 信息
      printf("module ip     :%s\r\n", wifi_uart_information.wifi_uart_local_ip);  // 模块 IP 地址
      printf("module port   :%s\r\n", wifi_uart_information.wifi_uart_local_port);// 模块 PORT 信息

      while(1)
      {
          data_length = wifi_uart_read_buffer(wifi_uart_get_data_buffer, sizeof(wifi_uart_get_data_buffer));
          if(data_length)                                                         // 如果接收到数据 则进行数据类型判断
          {
              link_id = strstr((char *)wifi_uart_get_data_buffer, "+IPD");
              if(NULL != link_id)                                                 // 判断数据格式是否是通过网络发送过来的数据
              {
                  printf("Get data: <%s>.\r\n", wifi_uart_get_data_buffer);
                  if(!wifi_uart_tcp_servers_send_buffer(wifi_uart_get_data_buffer, data_length, *(link_id + 5) - 0x30))
                  {
                      printf("send success.\r\n");
                      memset(wifi_uart_get_data_buffer, 0, data_length);          // 数据发送完成 清空数据
                  }
                  else
                  {
                      printf("%d bytes data send failed.\r\n", data_length);
                  }
              }
          }

          system_delay_ms(100);
      }
}


/*-------------------------------------------------------------------------------------------------------------------
// 函数简介     舵机测试
* 接线定义：
*                   ------------------------------------
*                   模块管脚             单片机管脚
*                   电机引脚             A2
*                   ------------------------------------
//-------------------------------------------------------------------------------------------------------------------*/
void servo_test(void) {
    char txt[16] = "";
    signed short duty = Servo_Center_Mid;


    ips200_clear();//清屏
    ServoCtrl(Servo_Center_Mid);//中值
    ips200_show_string(0, 0, "Servo Test");

    while(1)
    {
        //key = key_Scan();
        key_clear_all_state();
        key_scanner();
        if(key_get_state(KEY_1) == KEY_SHORT_PRESS)
        {
            duty = Servo_Center_Mid;
        }
        if(key_get_state(KEY_3) == KEY_SHORT_PRESS)
        {
            duty += 10;
        }
        if(key_get_state(KEY_2) == KEY_SHORT_PRESS)
        {
            duty -= 10;
        }
        if(key_get_state(KEY_4) == KEY_SHORT_PRESS )
        {
            ips200_clear();
            break;
        }

        ServoCtrl(duty);
        sprintf(txt, "Servo duty:%04d", duty);
        ips200_show_string(1, 2, txt);
        printf("Servo duty=%05d\r\n", duty);
       //电平翻转,LED闪烁
        gpio_toggle_level(LED2);
        system_delay_ms(200);
    }

}


/*-------------------------------------------------------------------------------------------------------------------
// 函数简介     电机测试
* 接线定义：
*                   ------------------------------------
*                   模块管脚             单片机管脚
*                   PWM1       C6
*                   PWM2       C7
*                   GPIO1      E3
*                   GPIO2      E4
*                   ------------------------------------
//-------------------------------------------------------------------------------------------------------------------*/
void motor_test(void) {
    int16_t duty = 800;
    char txt[16];
    key_clear_all_state();

    ips200_clear();
    ips200_show_string(0, 0, "Motor Test");


    while (1)
    {
        //key = key_Scan();

        if(key_get_state(KEY_1) == KEY_SHORT_PRESS)
        {
            duty = 800;
        }
        if(key_get_state(KEY_3) == KEY_SHORT_PRESS)
        {
            duty += 100;
        }
        if(key_get_state(KEY_2) == KEY_SHORT_PRESS)
        {
            duty -= 100;
        }
        if(key_get_state(KEY_1) == KEY_RELEASE)
        {
            ips200_clear();
            break;
        }
        MotorCtrl(duty, 0);
        sprintf(txt, "Motor duty:%04d", duty);
        ips200_show_string(1, 2, txt);
        printf("Motor duty:%04d\r\n", duty);
       //电平翻转,LED闪烁
        gpio_toggle_level(LED2);
        system_delay_ms(200);
    }
    while (1)
    {
        //key = key_Scan();

        if(key_get_state(KEY_1) == KEY_SHORT_PRESS)
        {
            duty = 800;
        }
        if(key_get_state(KEY_3) == KEY_SHORT_PRESS)
        {
            duty += 100;
        }
        if(key_get_state(KEY_2) == KEY_SHORT_PRESS)
        {
            duty -= 100;
        }
        if(key_get_state(KEY_1) == KEY_RELEASE)
        {
            return;
        }
        MotorCtrl(0, duty);
        sprintf(txt, "Motor duty:%04d", duty);
        ips200_show_string(1, 2, txt);
        printf("Motor duty:%04d\r\n", duty);
       //电平翻转,LED闪烁
        gpio_toggle_level(LED2);
        system_delay_ms(200);
    }
}


/*-------------------------------------------------------------------------------------------------------------------
// 函数简介     蜂鸣器测试
* 接线定义：
*                   ------------------------------------
*                   模块管脚             单片机管脚
*                              D9
*                   ------------------------------------
//-------------------------------------------------------------------------------------------------------------------*/
void beep_test(void)
{
    unsigned short freq = 4000;
    key_clear_all_state();
    while(1)
    {
        beep_set_freq(freq);
        system_delay_ms(100);
//        freq += 10;
//        if(fr'eq > 4500)
//        {
//            freq = 3500;
//        }
        //key = key_Scan();
        if(key_get_state(KEY_1) == KEY_SHORT_PRESS)
        {
            beep_set_sound(0);
            break;
        }
    }
}


void mt9v03x_test(void)
{
    ips200_clear();
    ips200_show_string(100, 250, "mt9v03x init.");
    key_scanner();
    key_clear_all_state();

    //threshold = (uint8)(flash_data_buffer[35] / 1000);
    threshold = 230;
    while(1)
    {

        if(mt9v03x_finish_flag)
        {
            ImageProcess();

            ips200_show_gray_image(0, 0, (const uint8 *)C_Image, ImageW, ImageH, ImageW*2, ImageH*2, threshold);
            ips200_show_int(100, 198, threshold, 3);
            ips200_show_string(100, 200, "thr");
            mt9v03x_finish_flag = 0;
        }
        //key = key_Scan();
        if (key_get_state(KEY_2) == KEY_SHORT_PRESS)
        {
            threshold += 1;
        }
        else if (key_get_state(KEY_1) == KEY_SHORT_PRESS)
        {
            threshold -= 1;
        }
        else if (key_get_state(KEY_3) == KEY_SHORT_PRESS)
        {
            threshold += 10;
        }
        else if (key_get_state(KEY_2) == KEY_LONG_PRESS)
        {
            threshold -= 10;
        }
        if(key_get_state(KEY_1) == KEY_LONG_PRESS)
        {
            ips200_clear();
            break;
        }
        else if (key_get_state(KEY_3) == KEY_SHORT_PRESS) {
            gpio_low(LED1);
            flash_data_buffer[35] = (uint32)(threshold * 1000);
            flash_erase_page(SECTION_INDEX, PAGE_INDEX);                              // 擦掉数据
            flash_write_page(SECTION_INDEX, PAGE_INDEX, flash_data_buffer, DATA_SIZE);      // 把缓冲区的数据写回去
            ips200_clear();
            Flag.Fix_Thr = true;
            gpio_high(LED1);
            break;
        }
    }
}



void mt9v03x_test_2(void)
{
    ips200_clear();
    ips200_show_string(0, 0, "mt9v03x init.");
    key_clear_all_state();

    light = (uint16)(flash_data_buffer[37] / 1000);


    while(1)
    {

        if(mt9v03x_finish_flag)
        {
            ImageProcess();
            ips200_show_gray_image(0, 0, (const uint8 *)Pixle, ImageW, ImageH, ImageW, ImageH, 0);
            ips200_show_int(100, 3, light, 3);
            ips200_show_string(100, 4, "light");
            mt9v03x_finish_flag = 0;
        }
        //key = key_Scan();
        if (key_get_state(KEY_2) == KEY_SHORT_PRESS)
        {
            light += 10;
            mt9v03x_set_exposure_time(light);
        }
        else if (key_get_state(KEY_1) == KEY_SHORT_PRESS)
        {
            light -= 10;
            mt9v03x_set_exposure_time(light);
        }
        else if (key_get_state(KEY_3) == KEY_SHORT_PRESS)
        {
            light += 100;
            mt9v03x_set_exposure_time(light);
        }
        else if (key_get_state(KEY_2) == KEY_LONG_PRESS)
        {
            light -= 100;
            mt9v03x_set_exposure_time(light);
        }
        if(key_get_state(KEY_1) == KEY_LONG_PRESS)
        {
            ips200_clear();
            break;
        }
        else if (key_get_state(KEY_3) == KEY_LONG_PRESS) {
            gpio_low(LED1);
            flash_data_buffer[37] = (uint32)(light * 1000);

            flash_erase_page(SECTION_INDEX, PAGE_INDEX);                              // 擦掉数据
            flash_write_page(SECTION_INDEX, PAGE_INDEX, flash_data_buffer, DATA_SIZE);      // 把缓冲区的数据写回去


            ips200_clear();
            gpio_high(LED1);
            break;
        }
    }
}



uint8 data_buffer[32];
uint8 data_len;
/*-------------------------------------------------------------------------------------------------------------------
// 函数简介     无线测试测试
* 接线定义：
*                   ------------------------------------
*                   模块管脚             单片机管脚

*                   ------------------------------------
//-------------------------------------------------------------------------------------------------------------------*/
void wireless_test(void)
{
    ips200_clear();
    ips200_show_string(0, 1, "wireless");
    wireless_uart_send_byte('\r');
    wireless_uart_send_byte('\n');
    wireless_uart_send_string("SEEKFREE wireless uart demo.\r\n");              // 初始化正常 输出测试信息
    key_clear_all_state();
    while(1)
    {
        // 此处编写需要循环执行的代码
        data_len = wireless_uart_read_buffer(data_buffer, 32);                    // 查看是否有消息 默认缓冲区是 WIRELESS_UART_BUFFER_SIZE 总共 64 字节
        if(data_len != 0)                                                       // 收到了消息 读取函数会返回实际读取到的数据个数
        {
            gpio_toggle_level(LED1);
            wireless_uart_send_buffer(data_buffer, data_len);                     // 将收到的消息发送回去
            memset(data_buffer, 0, 32);
            func_uint_to_str((char *)data_buffer, data_len);
            wireless_uart_send_string("\r\ndata len:");                                 // 显示实际收到的数据信息
            wireless_uart_send_buffer(data_buffer, strlen((const char *)data_buffer));    // 显示收到的数据个数
            wireless_uart_send_string(".\r\n");
        }
        else
        {
            wireless_uart_send_string("NO data\r\n");

        }
        system_delay_ms(50);
        // 此处编写需要循环执行的代码
        //key = key_Scan();
        if(key_get_state(KEY_1) == KEY_LONG_PRESS)
        {
            ips200_show_string(0, 1, "no");
            break;
        }
    }
}


/*-------------------------------------------------------------------------------------------------------------------
// 函数简介     编码器测试
* 接线定义：
*                   ------------------------------------
*                   模块管脚             单片机管脚

*                   ------------------------------------
//-------------------------------------------------------------------------------------------------------------------*/
//void encoder_test(void) {
//    char txt[32];

//    Encoder_Init(TIM2_ENCA_A15, TIM2_ENCB_B3);
//    Encoder_Init(TIM3_ENCA_B4, TIM3_ENCB_B5);

//    key_state_enum key = NO_KEY_PRESS;

//    ips200_clear();
//    ips200_show_string(ips200_MID, 0, "ips200 test");
//    while (1)
//    {
//        ECPULSE2 = Read_Encoder(TIM2);
//        ECPULSE3 = Read_Encoder(TIM3);


//        sprintf(txt, "Enc2: %05d;", ECPULSE2);
//        ips200_show_string(0, 1, txt);

//        sprintf(txt, "Enc3: %05d;", ECPULSE3);
//        ips200_show_string(0, 2, txt);


//        sprintf(txt, "Enc2=%05d,Enc3=%05d\r\n", ECPULSE2, ECPULSE3); // 串口输出
//        wireless_uart_send_string(txt);
//        system_delay_ms(200);//延时等待
//        key = key_Scan();
//        if(key == KEY_UP1_PRESS)
//        {
//            break;
//        }
//    }
//}
void encoder_test(void) {
    volatile int ECPULSE2,ECPULSE3;
    char txt[32];

    //Encoder_Init(TIM2_ENCA_A15, TIM2_ENCB_B3);
    //Encoder_Init(TIM3_ENCA_B4, TIM3_ENCB_B5);
    encoder_quad_init(TIM2_ENCODER,TIM2_ENCODER_CH1_P33_7,TIM2_ENCODER_CH2_P33_6);
    encoder_quad_init(TIM4_ENCODER,TIM4_ENCODER_CH1_P02_8,TIM4_ENCODER_CH2_P00_9);

    //key_state_enum key = NO_KEY_PRESS;
    ips200_init(IPS200_TYPE_SPI);
    ips200_clear();
    ips200_show_string(0, 0, "ips200 test");
    while (1)
    {
        ECPULSE2 = encoder_get_count(TIM2_ENCODER);
        ECPULSE3 = encoder_get_count(TIM4_ENCODER);


        sprintf(txt, "Enc2: %05d;", ECPULSE2);
        ips200_show_string(0, 100, txt);

        sprintf(txt, "Enc3: %05d;", ECPULSE3);
        ips200_show_string(0, 200, txt);


        //sprintf(txt, "Enc2=%05d,Enc3=%05d\r\n", ECPULSE2, ECPULSE3); // 串口输出
        //wireless_uart_send_string(txt);
        //system_delay_ms(200);//延时等待

        key_init(10);
        key_clear_all_state();
        key_scanner();
        //key = key_Scan();
        if(key_get_state(KEY_1) == KEY_SHORT_PRESS)
        {
            break;
        }
    }
}


#define PIT                         (CCU60_CH0) // 这个要从CCU60_CH0,CCU60_CH1,CCU61_CH0,CCU61_CH1里选一个，因为芯片换了，zf_driver_pit.h里的枚举类型也变了

//int16 encoder_data_1;

//void encoder_dir_test(void)
//{
//    char txt[32];

//    pit_ms_init(PIT, 100);

//    int16 encoder_data_1;
//    int16 encoder_data_2;

//    key_state_enum key = NO_KEY_PRESS;
//    ips200_clear();
//    ips200_show_string(ips200_MID, 0, "encoder test");
//    while (1)
//    {
//        encoder_data_1 = encoder_get_count(ENCODER_1);                              // 获取编码器计数
//        encoder_clear_count(ENCODER_1);

//        encoder_data_2 = -encoder_get_count(ENCODER_2);                              // 获取编码器计数
//        encoder_clear_count(ENCODER_2);

//        sprintf(txt, "Enc1: %4d", encoder_data_1);
//        ips200_show_string(0, 1, txt);

//        sprintf(txt, "Enc2: %4d", encoder_data_2);
//        ips200_show_string(0, 2, txt);

//        system_delay_ms(100);//延时等待
//        key = key_Scan();
//        if(key == KEY_UP1_PRESS)
//        {
//            break;
//        }
//    }
//}

void infrared_test(void)
{
    ips200_clear();
    ips200_show_string(0, 0, "infrared_test");
    gpio_init(INFRARED_PIN_A,GPI,1,GPI_PULL_UP);
    gpio_init(INFRARED_PIN_B,GPI,1,GPI_PULL_UP);
    key_clear_all_state();
    while(1)
    {
        if(!(gpio_get_level(INFRARED_PIN_B) == GPIO_HIGH))
        {
            ips200_show_string(0, 1, "BBBBB");
        }else {
            ips200_show_string(0, 1, "NoNoNo");
        }


        if (!(gpio_get_level(INFRARED_PIN_A) == GPIO_HIGH)) {
            ips200_show_string(0, 2, "AAAAA");
        }else{
            ips200_show_string(0, 2, "NoNoNo");
        }
        //key = key_Scan();
        if(key_get_state(KEY_1) == KEY_LONG_PRESS)
        {
           ips200_clear();
            break;
        }
    }

}

//uint16 pit_ms_count = 0;
void dl1a_test(void)
{
    ips200_show_string(0, 0, "dlla");
    key_clear_all_state();
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
        gpio_toggle_level(LED1);                                                // 翻转 LED 引脚输出电平 控制 LED 亮灭 初始化出错这个灯会闪的很慢
        //key = key_Scan();
        if(key_get_state(KEY_1) == KEY_LONG_PRESS)
        {
            break;
        }
    }
    pit_ms_init(PIT, 5);

    while(1)
    {
        // 此处编写需要循环执行的代码
        system_delay_ms(100);
        sprintf(txt1, "DL1A distance data: %5d\r\n", dl1b_distance_mm);
        ips200_show_string(0, 0, txt1);
        wireless_uart_send_string(txt1);
        gpio_toggle_level(LED2);                                                // 翻转 LED 引脚输出电平 控制 LED 亮灭

        system_delay_ms(200);
        // 此处编写需要循环执行的代码
        //key = key_Scan();
        if(key_get_state(KEY_1) == KEY_LONG_PRESS)
        {
            pit_disable(PIT);
            break;
        }
    }
}


#define ENCODER_TEST           (0)

void pit_hanlder (void)
{
#if ENCODER_TEST
    encoder_data_1 = encoder_get_count(ENCODER_1);                              // 获取编码器计数
    encoder_clear_count(ENCODER_1);                                             // 清空编码器计数
#else
    if(0 == pit_ms_count % 10)                                                  // 每 40ms 获取一次测距信息 周期 40 ms 频率 25Hz
    {
        dl1b_get_distance();                                                    // 测距调用频率不应高于 30Hz 周期不应低于 33.33ms
    }
    pit_ms_count = (pit_ms_count == 995) ? (0) : (pit_ms_count + 5);            // 1000ms 周期计数
#endif
}



void ele_mad_test(void)
{
    char txt[64];
    ips200_clear();
    ips200_show_string(0, 0, "ele_mad_test");
    int L1, L2, L3, L4;
    int L_max = 0, LM_max = 0, R_max = 0, RM_max = 0;
    key_clear_all_state();
    while (1)
    {
        L1 = adc_mean_filter_convert(L_ADC, 8);
        L2 = adc_mean_filter_convert(LM_ADC, 8);
        L3 = adc_mean_filter_convert(R_ADC, 8);
        L4 = adc_mean_filter_convert(RM_ADC, 8);

        if(L1 > L_max)
            L_max = L1;

        if(L2 > LM_max)
            LM_max = L2;

        if(L3 > R_max)
            R_max = L3;

        if(L4 > RM_max)
            RM_max = L4;


        sprintf(txt, "L: %4d;", L1);
        ips200_show_string(0, 7, txt);
        sprintf(txt, "L2: %4d;", L2);
        ips200_show_string(0, 5, txt);
        sprintf(txt, "R: %4d;", L3);
        ips200_show_string(0, 1, txt);
        sprintf(txt, "R2: %4d;", L4);
        ips200_show_string(0, 3, txt);

       // key = key_Scan();
        if(key_get_state(KEY_1) == KEY_LONG_PRESS)
        {
           ips200_clear();
            break;
        }

        if(key_get_state(KEY_3) == KEY_LONG_PRESS)
        {
            gpio_set_level(LED1, GPIO_LOW);

            flash_data_buffer[66] = (uint32)(L_max * 1000);
            flash_data_buffer[67] = (uint32)(LM_max * 1000);
            flash_data_buffer[68] = (uint32)(RM_max * 1000);
            flash_data_buffer[69] = (uint32)(R_max * 1000);

            flash_erase_page(SECTION_INDEX, PAGE_INDEX);                              // 擦掉数据
            flash_write_page(SECTION_INDEX, PAGE_INDEX, flash_data_buffer, DATA_SIZE);      // 把缓冲区的数据写回去


            gpio_set_level(LED1, GPIO_HIGH);
            ips200_clear();
             break;
        }

        system_delay_ms(100);//延时等待
    }

}


void blue_test(void)
{
    char buffer[32];
    double err = -10;


    while(1)
    {
        if(err < 120) err += 0.8;
        else err = -100;

        sprintf(buffer, "%8f", err);
        bluetooth_ch9141_send_string(buffer);
        system_delay_ms(200);
    }


}
