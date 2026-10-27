
#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "bsp_led.h"
#include <stdbool.h> 
#include "bsp_usart.h"

/*
 * t : 定时时间 
 * Ticks : 多少个时钟周期产生一次中断 
 * f : 时钟频率 72000000
 * t = Ticks * 1/f = (72000000/100000) * (1/72000000) = 10us 
 */ 

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	/* LED 端口初始化 */
	LED_GPIO_Config();

    // configure the tick as 1ms
	SysTick_Init();

    /*初始化USART 配置模式为 115200 8-N-1，不靠中断接收，靠标志位接收*/
    USART_Config();

	for(;;)
	{
        // control RGB led on/off every 1 sec
        static uint32_t system_time = 0u; 
        uint32_t system_time_previous = 0u; 

        bool led_control = false;

        system_time_previous = system_time;
        system_time = systick_get_tick();

        if ( !(system_time % 1000u) &&
                (system_time_previous % 1000u))
        {
           if (system_time / 1000u % 2)
           {
               led_control = ON;
           }
           else
           {
               led_control = OFF;
           }

           if (1u == (system_time / 1000 % 6) ||
                   2u == (system_time / 1000 % 6))
           {
               LED1(led_control);
           }
           else if (3u == (system_time / 1000 % 6) ||
                   4u == (system_time / 1000 % 6))
           {
               LED2(led_control);
           }
           else
           {
               LED3(led_control);
           }

           // capture bootloader entry command
           // Define the target string
           char target[] = "bootloader";
           
           // Call the function, passing the string and its length (excluding the null terminator)
           bool found = uart_check_contains_flash(target, sizeof(target) - 1);
           
           if (found)
           {
               // Trigger the software reset
               NVIC_SystemReset();
           }
           else
           {
               usart_reset_buffer_cursor();
           }
        }

	} 
	
}
