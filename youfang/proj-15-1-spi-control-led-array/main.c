#include "MAX7219.h"
#include "stm32f10x.h" // Device header

uint8_t smile[8] = {0x3C, 0x42, 0xA5, 0x81, 0xA5, 0x99, 0x42, 0x3C};
int main(void)
{
    MAX7219_Init();

    while (1)
    {
        uint8_t i = 0;
        for (i = 1; i <= 8; i++)
        {
            Write_MAX7219(i, smile[i - 1]);
        }
    }
}
