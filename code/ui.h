/*
 * ui.h
 *
 *  Created on: 2023年4月28日
 *      Author: Kern
 */

#ifndef UI_H_
#define UI_H_

#define Char_num   21  //简单字符个数宏定义
#define first_menu_num 6
#define second_menu_num_1 6

#define KEY_UP_PIN     D11
#define KEY_DOWN_PIN   D12
#define KEY_LEFT_PIN   D10
#define KEY_RIGHT_PIN  A8
#define KEY_ADD_PIN    D8
#define KEY_DEC_PIN    D9

#define KEY_UP_STATE          gpio_get_level(KEY_UP_PIN)
#define KEY_DOWN_STATE        gpio_get_level(KEY_DOWN_PIN)
#define KEY_LEFT_STATE        gpio_get_level(KEY_LEFT_PIN)
#define KEY_RIGHT_STATE       gpio_get_level(KEY_RIGHT_PIN)
#define KEY_ADD_STATE         gpio_get_level(KEY_ADD_PIN)
#define KEY_DEC_STATE         gpio_get_level(KEY_DEC_PIN)

extern float UI_Param[10][10];

void show_main_window           (uint8 line);
void show_ui                    (void);

#endif /* UI_H_ */
