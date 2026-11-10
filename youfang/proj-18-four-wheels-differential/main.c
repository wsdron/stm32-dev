#include "bsp_SysTick.h"
#include "Motor.h"
#include "OLED.h"
#include "key.h"
#include "bsp_usart.h"
#include "stm32f10x.h" // Device header

uint8_t Num;

int main(void)
{
    /* Initialize systick to 1ms, enable its interrupt */
    SysTick_Init();

    Motor_Init();

    USART_Config();

    OLED_Init();
    OLED_ShowString(1, 1, "App running");

    Key_Init();
    while (1)
    {
        Num = Key_GetNum();
        if (Num == 1)
        {
            Car_TurnLeft(500);
        }
        else if (Num == 2)
        {
            Car_TurnRight(500);
        }
        else if (Num == 3)
        {
            Car_TransLeft(500);
        }
        else if (Num == 4)
        {
            Car_TransRight(500);
        }

        static uint32_t system_time = 0u;
        uint32_t system_time_previous = 0u;

        system_time_previous = system_time;
        system_time = systick_get_tick();

        // check if any uart msg received every 1000ms
        if (!(system_time % 1000u) && (system_time_previous % 1000u))
        {
            // indicate the status of waiting for flash request
            Usart_SendString(DEBUG_USARTx, "heart_beat_1sec\r\n");

            char uart_buff[32] = {0};
            uint8_t uart_buff_size = 0u;

            if(usart_get_buffer(uart_buff, &uart_buff_size))
            {
                OLED_ShowString(2, 1, uart_buff);
            }
            else
            {
                OLED_ShowString(2, 1,"empty buffer");
            }
        }
    }
}
