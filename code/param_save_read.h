/*
 * param_save_read.h
 *
 *  Created on: May 17, 2023
 *      Author: kern
 */

#ifndef PARAM_SAVE_READ_H_
#define PARAM_SAVE_READ_H_


#include "zf_common_headfile.h"


#define SECTION_INDEX                    63                                         // ���һ������
#define PAGE_INDEX                       0                                          // page1����Ȧ�ĵ�    page0��һ������Ĳ��Ե�
#define DATA_SIZE                        1024/sizeof(uint32)                        // 32bit���ݴ洢 �������1024/4��С  =  256

extern uint32 flash_data_buffer[DATA_SIZE];
extern int16 imu_gyro_z_0;
extern int16 imu_gyro_x_0;

void ui_load_to_data                (void);
void param_save                     (bool is_ui);
void param_read                     (void);
short Data_init                     (void);

#endif /* PARAM_SAVE_READ_H_ */
