#include "main.h"
#include "bsp_key.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "flash.h"
#include "stm32f10x.h"
#include "xmodem.h"

/**
 * @brief  The application entry point.
 *
 * @retval None
 */
int main(void)
{
    /* Initialize all configured peripherals */
    /* LED 端口初始化 */
    LED_GPIO_Config();
    LED3(ON);

    /*初始化USART 配置模式为 115200 8-N-1，不靠中断接收，靠标志位接收*/
    USART_Config();

    // init the key GPIO
    Key_GPIO_Config();

    /* Send welcome message on startup. */
    Usart_SendString(DEBUG_USARTx, "\n\r================================\n\r");
    Usart_SendString(DEBUG_USARTx, "UART Bootloader\n\r");
    Usart_SendString(DEBUG_USARTx, "================================\n\r\n\r");

    /* If the button is pressed, then jump to the user application,
     * otherwise stay in the bootloader. */
    if (Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_OFF)
    {
        Usart_SendString(DEBUG_USARTx, "Jumping to user application...\n\r");
        flash_jump_to_app();
    }

    /* Turn off the blue LED, turn on green LED to indicate, that we are in
     * bootloader mode.*/
    LED3(OFF);
    LED2(ON);

    while (1)
    {
        /* Ask for new data and start the Xmodem protocol. */
        Usart_SendString(DEBUG_USARTx,
                         "Please send a new binary file with Xmodem protocol "
                         "to update the firmware.\n\r");
        xmodem_receive();
        /* We only exit the xmodem protocol, if there are any errors.
         * In that case, notify the user and start over. */
        Usart_SendString(DEBUG_USARTx, "\n\rFailed... Please try again.\n\r");
    }
}
