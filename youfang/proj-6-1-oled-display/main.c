#include "LED.h"
#include "OLED.h"
#include "stm32f10x.h"

int main(void)
{
    OLED_Init();
    OLED_ShowChar(1, 1, 'A');
    OLED_ShowString(1, 3, "YourFun!");
    OLED_ShowNum(2, 1, 2397, 4);
    OLED_ShowSignedNum(2, 6, -2397, 4);
    OLED_ShowHexNum(3, 1, 0xff01, 4);
    OLED_ShowBinNum(4, 1, 0xff01, 16);
    while (1)
    {
    }
}
