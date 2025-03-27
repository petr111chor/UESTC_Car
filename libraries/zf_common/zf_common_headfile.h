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
* �ļ�����          zf_common_headfile
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

#ifndef _zf_common_headfile_h_
#define _zf_common_headfile_h_

//===================================================C���� ������===================================================
#include "math.h"
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
//===================================================C���� ������===================================================

//===================================================оƬ SDK �ײ�===================================================
#include "ifxAsclin_reg.h"
#include "SysSe/Bsp/Bsp.h"
#include "IfxCcu6_Timer.h"
#include "IfxScuEru.h"

//#include "ch32v30x_adc.h"
//#include "ch32v30x_bkp.h"
//#include "ch32v30x_can.h"
//#include "ch32v30x_crc.h"
//#include "ch32v30x_dac.h"
//#include "ch32v30x_dbgmcu.h"
//#include "ch32v30x_dma.h"
//#include "ch32v30x_exti.h"
//#include "ch32v30x_flash.h"
//#include "ch32v30x_fsmc.h"
//#include "ch32v30x_gpio.h"
//#include "ch32v30x_i2c.h"
//#include "ch32v30x_iwdg.h"
//#include "ch32v30x_pwr.h"
//#include "ch32v30x_rcc.h"
//#include "ch32v30x_rtc.h"
//#include "ch32v30x_sdio.h"
//#include "ch32v30x_spi.h"
//#include "libraries/my_lib/sdk/ch32v30x_tim.h"
//#include "ch32v30x_usart.h"
//#include "ch32v30x_wwdg.h"
//===================================================оƬ SDK �ײ�===================================================

//====================================================��Դ�⹫����====================================================
#include "zf_common_typedef.h"
#include "zf_common_clock.h"
#include "zf_common_debug.h"
#include "zf_common_fifo.h"
#include "zf_common_font.h"
#include "zf_common_function.h"
#include "zf_common_interrupt.h"
#include "isr_config.h"
//====================================================��Դ�⹫����====================================================

//===================================================оƬ����������===================================================
#include "zf_driver_adc.h"
#include "zf_driver_delay.h"
#include "zf_driver_dma.h"
#include "zf_driver_encoder.h"
#include "zf_driver_exti.h"
#include "zf_driver_flash.h"
#include "zf_driver_gpio.h"
#include "zf_driver_pit.h"
#include "zf_driver_pwm.h"
#include "zf_driver_soft_iic.h"
#include "zf_driver_spi.h"
#include "zf_driver_soft_spi.h"
#include "zf_driver_uart.h"
#include "zf_driver_timer.h"
//===================================================оƬ����������===================================================

//===================================================����豸������===================================================
#include "zf_device_absolute_encoder.h"
#include "zf_device_ble6a20.h"
#include "zf_device_bluetooth_ch9141.h"
#include "zf_device_gnss.h"
#include "zf_device_camera.h"
#include "zf_device_dl1a.h"
#include "zf_device_dl1b.h"
#include "zf_device_icm20602.h"
#include "zf_device_imu660ra.h"
#include "zf_device_imu963ra.h"
#include "zf_device_ips114.h"
#include "zf_device_ips200.h"
#include "zf_device_key.h"
#include "zf_device_menc15a.h"
#include "zf_device_mpu6050.h"
#include "zf_device_mt9v03x.h"
#include "zf_device_oled.h"
#include "zf_device_ov7725.h"
#include "zf_device_scc8660.h"
#include "zf_device_tft180.h"
#include "zf_device_tsl1401.h"
#include "zf_device_type.h"
#include "zf_device_uart_receiver.h"
#include "zf_device_virtual_oscilloscope.h"
#include "zf_device_wifi_uart.h"
#include "zf_device_wifi_spi.h"
#include "zf_device_wireless_uart.h"
//===================================================����豸������===================================================

//====================================================Ӧ�������=====================================================
#include "seekfree_assistant.h"
#include "seekfree_assistant_interface.h"
//====================================================Ӧ�������=====================================================

//=====================================================�û���=======================================================
#include "adc.h"
#include "all_init.h"
#include "angleprocess.h"
#include "beep.h"
//#include "encoder.h"
#include "HANA_math.h"
#include "image.h"
#include "imu.h"
#include "module_test.h"
#include "motorServo.h"
#include "param_save_read.h"
#include "pid.h"
#include "run_car.h"
#include "speedprocess.h"
#include "ui.h"
#include "variables.h"
#include "pidpid.h"
#include "motor.h"
#include "wifi.h"

//#include <diskio.h>
//#include <bmp.h>
//#include <ff.h>
//#include <ffconf.h>
//#include <font.h>
//#include <mmc_sd.h>
//#include <spi.h>
//=====================================================�û���=======================================================
#endif

