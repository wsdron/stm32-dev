#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"
#include <stdint.h> 

void SysTick_Init(void);
void Delay_us(__IO uint32_t nTime);
#define Delay_ms(x) Delay_us(100*x)	 //单位ms

void SysTick_Delay_Us( __IO uint32_t us);
void SysTick_Delay_Ms( __IO uint32_t ms);

void systick_tick_increment(void);
uint32_t systick_get_tick(void);

#endif /* __SYSTICK_H */
