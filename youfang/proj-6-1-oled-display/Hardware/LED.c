#include "stm32f10x.h"

void LED_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,
                        ENABLE); // PA15复用做普通IO口输出

    GPIO_InitStructure.GPIO_Pin =
        GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
    GPIO_WriteBit(GPIOA, GPIO_Pin_11, Bit_SET);
    GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_SET);
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET);
}

void LED1_ON()
{
    // GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_RESET);
    GPIO_ResetBits(GPIOA, GPIO_Pin_8);
}

void LED1_OFF()
{
    // GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_SET);
    GPIO_SetBits(GPIOA, GPIO_Pin_8);
}

void LED1_Turn()
{
    if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_8) == 0)
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
    }
    else
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
    }
}

void LED2_ON()
{
    // GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_RESET);
    GPIO_ResetBits(GPIOA, GPIO_Pin_11);
}

void LED2_OFF()
{
    // GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_SET);
    GPIO_SetBits(GPIOA, GPIO_Pin_11);
}

void LED2_Turn()
{
    if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_11) == 0)
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_11, Bit_SET);
    }
    else
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_11, Bit_RESET);
    }
}

void LED3_ON()
{
    // GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_RESET);
    GPIO_ResetBits(GPIOA, GPIO_Pin_12);
}

void LED3_OFF()
{
    // GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_SET);
    GPIO_SetBits(GPIOA, GPIO_Pin_12);
}

void LED3_Turn()
{
    if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_12) == 0)
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_SET);
    }
    else
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_RESET);
    }
}

void LED4_ON()
{
    // GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_RESET);
    GPIO_ResetBits(GPIOA, GPIO_Pin_15);
}

void LED4_OFF()
{
    // GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_SET);
    GPIO_SetBits(GPIOA, GPIO_Pin_15);
}

void LED4_Turn()
{
    if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_15) == 0)
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET);
    }
    else
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_RESET);
    }
}
