/*
 * beep.c
 *
 *  Created on: 2023Äê5ÔÂ8ÈÕ
 *      Author: Kern
 */

#include "zf_common_headfile.h"

#define BEEP_FREQ               (1000 )


void beep_init(void)
{
    pwm_init(Beep_PWM_Pin, BEEP_FREQ, 0);
}


void beep_set_sound(beep_state_enum state)
{
    if(state == BEEP_ON)
    {
        pwm_set_duty(Beep_PWM_Pin,5000);
    }
    else if(state == BEEP_OFF)
    {
        pwm_set_duty(Beep_PWM_Pin,0);
    }
}


void beep_sound_point(void)
{
    beep_set_sound(BEEP_ON);
    system_delay_ms(100);
    beep_set_sound(BEEP_OFF);
}


void beep_set_freq(int freq)
{
    pwm_set_duty(Beep_PWM_Pin, freq);
}





