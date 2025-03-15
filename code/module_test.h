/*
 * module_test.h
 *
 *  Created on: 2023Äê5ÔÂ5ÈÕ
 *      Author: Kern
 */

#ifndef MODULE_TEST_H_
#define MODULE_TEST_H_

extern uint8 data_buffer[32];
extern uint8 data_len;

#define INFRARED_PIN_A                      (P02_0)//A7
#define INFRARED_PIN_B                      (P02_1)//A3



void led_test                               (void);
void oled_test                              (void);
void wifi_test                              (void);
void servo_test                             (void);
void motor_test                             (void);
void beep_test                              (void);
void mt9v03x_test                           (void);
void mt9v03x_test_2                         (void);
void wireless_test                          (void);
void encoder_dir_test                       (void);
void encoder_test                           (void);
void infrared_test                          (void);
void dl1a_test                              (void);
void ele_mad_test                           (void);
void blue_test                              (void);

#endif /* MODULE_TEST_H_ */
