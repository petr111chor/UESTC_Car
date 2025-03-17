/*
 * beep.h
 *
 *  Created on: 2023Äê5ÔÂ8ÈÕ
 *      Author: Kern
 */

#ifndef BEEP_H_
#define BEEP_H_

#define   Beep_PWM_Pin              (ATOM3_CH0_P33_10)


typedef enum
{
    BEEP_OFF,
    BEEP_ON,
}beep_state_enum;



void beep_init                              (void);
void beep_set_sound                         (beep_state_enum state);
void beep_set_freq                          (int freq);
void beep_sound_point                       (void);

#endif /* BEEP_H_ */

