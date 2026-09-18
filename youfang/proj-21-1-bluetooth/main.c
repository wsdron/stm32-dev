#include "Delay.h"
#include "Motor.h"
#include "Usart.h"
#include "OLED.h"
#include "stm32f10x.h" // Device header
#include <string.h>

#define MOTOR_USED (0u)

#if MOTOR_USED == 1u
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Usart1_Init();
    Motor_Init();
    OLED_Init();

    while (1)
    {
        Usart1_SendByte(0x01);
        Delay_ms(1000);

        if (Usart1_GetRxFlag())
        {
            if (strcmp(Rx_Packet, "CAR_F") == 0)
            {
                Car_Forward(200);
            }
            else if (strcmp(Rx_Packet, "CAR_B") == 0)
            {
                Car_Backward(200);
            }
            else if (strcmp(Rx_Packet, "CAR_TF") == 0)
            {
                Car_TurnLeft(200);
            }
            else if (strcmp(Rx_Packet, "CAR_TR") == 0)
            {
                Car_TurnRight(200);
            }
            else if (strcmp(Rx_Packet, "CAR_TSF") == 0)
            {
                Car_TransLeft(200);
            }
            else if (strcmp(Rx_Packet, "CAR_TSR") == 0)
            {
                Car_TransRight(200);
            }
            else
            {
                Car_Stop();
            }
        }
    }
}

#else
int main(void)
{
    Usart1_Init();
    OLED_Init();

    while (1)
    {
        Usart1_SendByte(0x01);
        Delay_ms(1000);

        if (Usart1_GetRxFlag())
        {
            OLED_ShowString(1, 3, Rx_Packet);
        }
    }
}
#endif
