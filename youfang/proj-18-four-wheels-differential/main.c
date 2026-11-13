#include "bsp_SysTick.h"
#include "Motor.h"
#include "OLED.h"
#include "command.h"
#include "bsp_usart.h"
#include "stm32f10x.h" // Device header

int main(void)
{
    /* Initialize systick to 1ms, enable its interrupt */
    SysTick_Init();

    Motor_Init();

    USART_Config();

    OLED_Init();
    OLED_ShowString(1, 1, "App running");
    OLED_ShowString(3, 1,"cmd:");

    while (1)
    {

        static uint32_t system_time = 0u;
        uint32_t system_time_previous = 0u;

        system_time_previous = system_time;
        system_time = systick_get_tick();

        // check if any uart msg received every 1000ms
        if (!(system_time % 1000u) && (system_time_previous % 1000u))
        {
            // indicate the status of app software
            Usart_SendByte(DEBUG_USARTx, 'A');

            //Usart_SendString(DEBUG_USARTx, "heart_beat_1sec\r\n");

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

            // vehicle control
            static vehicle_mode command_prev = vehicle_mode_undefined;
            vehicle_mode command = vehicle_mode_undefined;

            command = vehicle_command_detection();

            if (command != command_prev)
            {
                char clear[12] = "            ";
                OLED_ShowString(3, 5, clear);
                char command_str[32] = {0};
                get_command_value(command, command_str);
                OLED_ShowString(3, 5, command_str);
            }
            command_prev  = command;

            if (vehicle_mode_turn_left == command)
            {
                Car_TurnLeft(500);
            }
            else if (vehicle_mode_turn_right == command)
            {
                Car_TurnRight(500);
            }
            else if (vehicle_mode_trans_left == command)
            {
                Car_TransLeft(500);
            }
            else if (vehicle_mode_trans_right == command)
            {
                Car_TransRight(500);
            }
            else if (vehicle_mode_stop == command)
            {
                Car_Stop();
            }


            // OTA request detection
            char target[] = "reset";

            // Call the function, passing the string and its length (excluding
            // the null terminator)
            bool found = uart_check_contains_flash(target, sizeof(target) - 1);

            if (found)
            {
                Car_Stop();

                // Trigger the software reset
                NVIC_SystemReset();
            }
            else
            {
                usart_reset_buffer_cursor();
            }

            usart_reset_buffer_cursor();
        }
    }
}
