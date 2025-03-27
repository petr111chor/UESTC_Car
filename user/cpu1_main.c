/*********************************************************************************************************************
* TC377 Opensourec Library ����TC377 ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
*
* ���ļ��� TC377 ��Դ���һ����
*
* TC377 ��Դ�� ��������
* �����Ը��������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù������֤��������
* �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����/���޸���
*
* ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı�֤
* ����û�������������Ի��ʺ��ض���;�ı�֤
* ����ϸ����μ� GPL
*
* ��Ӧ�����յ�����Դ���ͬʱ�յ�һ�� GPL �ĸ���
* ���û�У������<https://www.gnu.org/licenses/>
*
* ����ע����
* ����Դ��ʹ�� GPL3.0 ��Դ���֤Э�� �����������Ϊ���İ汾
* �������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ���֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
*
* �ļ�����          cpu1_main
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          ADS v1.10.2
* ����ƽ̨          TC377TP
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����              ����                ��ע
* 2022-11-03       pudding            first version
********************************************************************************************************************/

#include "zf_common_headfile.h"
#pragma section all "cpu1_dsram"
// ���������#pragma section all restore���֮���ȫ�ֱ���������CPU1��RAM��

// �������ǿ�Դ��չ��� ��������ֲ���߲��Ը���������
// �������ǿ�Դ��չ��� ��������ֲ���߲��Ը���������
// �������ǿ�Դ��չ��� ��������ֲ���߲��Ը���������

// **************************** �������� ****************************
char send_str[32] = {0};
void core1_main(void)
{
    disable_Watchdog();                     // �رտ��Ź�
    interrupt_global_enable(0);             // ��ȫ���ж�
    // �˴���д�û����� ���������ʼ�������



    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIRELESS_UART);

    // �˴���д�û����� ���������ʼ�������
    cpu_wait_event_ready();                 // �ȴ����к��ĳ�ʼ�����
    while (TRUE)
    {
        // �˴���д��Ҫѭ��ִ�еĴ���
        if(show_img_flag)
        {
            // ԭʼͼ��
            ips200_show_gray_image(25+ImageW, 50, (const uint8 *)C_Image, ImageW, ImageH, ImageW, ImageH, 0);
            // ��ֵ��ͼ��
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
            //turn_Update(); //��������Ƕ�
            ips200_show_int(150,230,servo_param.Servo_filter,3);

            for(uint8 i=0;i<9;i++){
                ips200_show_int(25*i+5, 150, image.cam_finalCenterERR[i], 2);
            }
            ips200_show_string(10,240,"turn_value"); ips200_show_int(110,240,ssss,3);
            ips200_show_string(10,260,"l_speed"); ips200_show_int(110,260,motor_l.encoder_speed,3);
            ips200_show_string(10,280,"r_speed"); ips200_show_int(110,280,motor_r.encoder_speed,3);
            ips200_show_string(10,300,"difference"); ips200_show_int(110,300,motor_l.encoder_speed-motor_r.encoder_speed,3);
/*            //���ߴ��ڵ���
                       wireless_uart_init();
                       seekfree_assistant_data_analysis();
                       for(uint8_t i = 0; i < SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT; i++)
                               {
                                   // ���±�־λ
                                   if(seekfree_assistant_parameter_update_flag[i])
                                   {
                                       seekfree_assistant_parameter_update_flag[i] = 0;

                                       // ͨ������ת���ڷ�����Ϣ
                                       sprintf(send_str,"receive data channel : %d ", i);
                                       wireless_uart_send_buffer((uint8 *)send_str,strlen(send_str));
                                       sprintf(send_str,"data : %.2f \r\n", seekfree_assistant_parameter[i]);
                                       wireless_uart_send_buffer((uint8 *)send_str,strlen(send_str));
                                   }
                               }
                       //������ֵ��
                               //para = seekfree_assistant_parameter[0];

            //system_delay(1000);
*/            printf("%d,%d,%d,%d\n",motor_l.encoder_speed,motor_r.encoder_speed,motor_l.target_speed,motor_r.target_speed);

        }

        //Final_Motor_Control(60,0.6,50,30);


        // �˴���д��Ҫѭ��ִ�еĴ���
    }
}
#pragma section all restore
