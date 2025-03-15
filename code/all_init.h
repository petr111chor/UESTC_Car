/*
 * all_init.h
 *
 *  Created on: 2023Äê4ÔÂ27ÈÕ
 *      Author: Kern
 */

#ifndef ALL_INIT_H_
#define ALL_INIT_H_
#include "zf_common_headfile.h"


#define LED1                    (P20_9 )//D12
#define LED2                    (P20_8 )//D15
#define CHARGE_PIN              (P00_2 )//B7
#define CHARGE_ON               gpio_high(CHARGE_PIN)
#define CHARGE_OFF              gpio_low(CHARGE_PIN)


void all_init(void);

#endif /* ALL_INIT_H_ */
