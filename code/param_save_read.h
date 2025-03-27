/*
 * param_save_read.h
 *
 *  Created on: May 17, 2023
 *      Author: kern
 */

#ifndef PARAM_SAVE_READ_H_
#define PARAM_SAVE_READ_H_


#include "zf_common_headfile.h"


#define SECTION_INDEX                    63                                         // 最后一个扇区
#define PAGE_INDEX                       0                                          // page1存整圈的点    page0寸一个弯道的测试点
#define DATA_SIZE                        1024/sizeof(uint32)                        // 32bit数据存储 缓冲就是1024/4大小  =  256

extern uint32 flash_data_buffer[DATA_SIZE];
extern int16 imu_gyro_z_0;
extern int16 imu_gyro_x_0;

void ui_load_to_data                (void);
void param_save                     (bool is_ui);
void param_read                     (void);
short Data_init                     (void);

#endif /* PARAM_SAVE_READ_H_ */
