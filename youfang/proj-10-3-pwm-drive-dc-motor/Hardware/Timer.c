#include "stm32f10x.h"

void Timer2_Init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

	TIM_InternalClockConfig(TIM2);
//	TIM_TimeBaseInitTypeDef	TIM_TimeBaseInitStruct;
//	TIM_TimeBaseInitStruct.TIM_ClockDivision =TIM_CKD_DIV1;
//	TIM_TimeBaseInitStruct.TIM_CounterMode =TIM_CounterMode_Up;
//	TIM_TimeBaseInitStruct.TIM_Period = ;
//	TIM_TimeBaseInitStruct.TIM_Prescaler =;
//	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
//	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
}
