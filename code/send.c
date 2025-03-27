/*
 * send.c
 *
 *  Created on: 2025年3月27日
 *      Author: yolo
 */

#include "zf_common_headfile.h"

void wireless_send_int(int number){
    //无线串口发送整型数据
    wireless_uart_send_byte('\r');
    wireless_uart_send_byte('\n');
    char ascii_buffer[30];
    sprintf(ascii_buffer, "%d", number);
    uint32_t len = strlen(ascii_buffer);
    wireless_uart_send_buffer((const uint8_t *)ascii_buffer, len);
}

void wireless_send_double(double number){
    //无线串口发送浮点数据
    wireless_uart_send_byte('\r');
    wireless_uart_send_byte('\n');
    char ascii_buffer[30];
    sprintf(ascii_buffer, "%.3f", number);
    uint32_t len = strlen(ascii_buffer);
    wireless_uart_send_buffer((const uint8_t *)ascii_buffer, len);
}

void wireless_send_string(char number){
    //无线串口发送字符数据
    char byte[2];
    byte[0] = number;
    byte[1] = '\0';

    wireless_uart_send_byte('\r');
    wireless_uart_send_byte('\n');
    wireless_uart_send_string(byte);
}

void wireless_send_imag(){
    //无线串口发送图像
    wireless_uart_send_image(&mt9v03x_image[0][0], MT9V03X_IMAGE_SIZE);
}


