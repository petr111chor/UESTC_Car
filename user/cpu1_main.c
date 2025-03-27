/*********************************************************************************************************************
* TC377 Opensourec Library 即（TC377 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是 TC377 开源库的一部分
*
* TC377 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          cpu1_main
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          ADS v1.10.2
* 适用平台          TC377TP
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2022-11-03       pudding            first version
********************************************************************************************************************/

#include "zf_common_headfile.h"
#pragma section all "cpu1_dsram"
// 将本语句与#pragma section all restore语句之间的全局变量都放在CPU1的RAM中

// 本例程是开源库空工程 可用作移植或者测试各类内外设
// 本例程是开源库空工程 可用作移植或者测试各类内外设
// 本例程是开源库空工程 可用作移植或者测试各类内外设

// **************************** 代码区域 ****************************
char send_str[32] = {0};
void core1_main(void)
{
    disable_Watchdog();                     // 关闭看门狗
    interrupt_global_enable(0);             // 打开全局中断
    // 此处编写用户代码 例如外设初始化代码等



    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIRELESS_UART);

    // 此处编写用户代码 例如外设初始化代码等
    cpu_wait_event_ready();                 // 等待所有核心初始化完毕
    while (TRUE)
    {
        // 此处编写需要循环执行的代码
        if(show_img_flag)
        {
            // 原始图像
            ips200_show_gray_image(25+ImageW, 50, (const uint8 *)C_Image, ImageW, ImageH, ImageW, ImageH, 0);
            // 二值化图像
            for (uint8 i = 0;i<ImageH;i++){
                for (uint8 j = 0;j<ImageW;j++){
                    if(Pixle[i][j]==true)
                        ips200_draw_point(j+25,i+50,RGB565_WHITE);
                    else
                        ips200_draw_point(j+25,i+50,RGB565_BLACK);
                }
            }
            ips200_show_int(25, 225, threshold, 3);
            ips200_show_string(25, 200, "threshold:");
            ips200_show_int(110, 225, light, 3);
            ips200_show_string(110, 200, "light:");
            show_line();
            //turn_Update(); //舵机调整角度
            ips200_show_int(150,230,servo_param.Servo_filter,3);

            for(uint8 i=0;i<9;i++){
                ips200_show_int(25*i+5, 150, image.cam_finalCenterERR[i], 2);
            }
            ips200_show_string(10,240,"turn_value"); ips200_show_int(110,240,ssss,3);
            ips200_show_string(10,260,"l_speed"); ips200_show_int(110,260,motor_l.encoder_speed,3);
            ips200_show_string(10,280,"r_speed"); ips200_show_int(110,280,motor_r.encoder_speed,3);
            ips200_show_string(10,300,"difference"); ips200_show_int(110,300,motor_l.encoder_speed-motor_r.encoder_speed,3);
/*            //无线串口调参
                       wireless_uart_init();
                       seekfree_assistant_data_analysis();
                       for(uint8_t i = 0; i < SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT; i++)
                               {
                                   // 更新标志位
                                   if(seekfree_assistant_parameter_update_flag[i])
                                   {
                                       seekfree_assistant_parameter_update_flag[i] = 0;

                                       // 通过无线转串口发送信息
                                       sprintf(send_str,"receive data channel : %d ", i);
                                       wireless_uart_send_buffer((uint8 *)send_str,strlen(send_str));
                                       sprintf(send_str,"data : %.2f \r\n", seekfree_assistant_parameter[i]);
                                       wireless_uart_send_buffer((uint8 *)send_str,strlen(send_str));
                                   }
                               }
                       //参数赋值：
                               //para = seekfree_assistant_parameter[0];

            //system_delay(1000);
*/            printf("%d,%d,%d,%d\n",motor_l.encoder_speed,motor_r.encoder_speed,motor_l.target_speed,motor_r.target_speed);

        }

        //Final_Motor_Control(60,0.6,50,30);


        // 此处编写需要循环执行的代码
    }
}
#pragma section all restore
