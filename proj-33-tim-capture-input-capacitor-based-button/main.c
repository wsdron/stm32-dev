// TIM—通用-捕获-电容按键检测 应用
#include "bsp_SysTick.h"
#include "bsp_beep.h"
#include "bsp_led.h"
#include "bsp_tpad.h"
#include "bsp_usart.h"
#include "stm32f10x.h"

/**
 * @brief  主函数
 * @param  无
 * @retval 无
 */
int main(void)
{
    // uint16_t temp;
    /* led 端口配置 */
    LED_GPIO_Config();

    /* 蜂鸣器初始化 */
    Beep_Init();

    /* 串口初始化 */
    USART_Config();
    printf("\r\n capacitor based button press detection expreiment \r\n");
    printf("\r\n when button pressed, it will beep \r\n");

    // 初始化电容按键
    while (TPAD_Init())
    {
    }

    printf("\r\n now, let's calibrate the button, please keep it pressed, then send usart '1' \r\n");
    while ('1' != usart_get_char())
    {
    }

    // calibrate the button capacitor charge time when pressed
    while (TPAD_calibrate_charge_time_when_pressed())
    {
    }

    printf("\r\n lift the press, send usart '2' to start button detection \r\n");
    while ('2' != usart_get_char())
    {
    }
    
    while (1)
    {
        if (TPAD_Scan() == TPAD_ON)
        {
            BEEP_ON();
            SysTick_Delay_Ms(25);
            BEEP_OFF();
        }
    }
}
/*********************************************END OF FILE**********************/
