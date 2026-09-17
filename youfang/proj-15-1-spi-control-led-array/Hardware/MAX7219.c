#include "MAX7219.h"
#include "SPI.h"
#include "stm32f10x.h" // Device header
#include "sys.h"

void MAX7219_Init()
{
    SPI1_Init();
    MAX7219_CS_Init();
    Write_MAX7219(0x0C, 0x01);
    Write_MAX7219(0x0F, 0x00);
    Write_MAX7219(0x0A, 0x03);
    Write_MAX7219(0x0B, 0x07);
    Write_MAX7219(0x09, 0x00);
}

void Write_MAX7219(uint8_t addr, uint8_t data)
{
    PAout(4) = 0;
    SPI1_ReadWriteByte(addr);
    SPI1_ReadWriteByte(data);
    PAout(4) = 1;
}

void MAX7219_CS_Init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_SetBits(GPIOA, GPIO_Pin_4);
}
