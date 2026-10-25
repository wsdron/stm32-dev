#include "main.h"
#include "bsp_key.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "flash.h"
#include "stm32f10x.h"
#include "xmodem.h"
#include "bsp_SysTick.h"

/**
 * @brief  Converts a 32-bit unsigned integer to an 8-character hex string.
 * @param  value: The 32-bit unsigned integer to convert.
 * @param  result: Pointer to output buffer (must be at least 9 bytes for '\0').
 * @param  uppercase: Set to 1 for "00000011", 0 for lowercase.
 */
void uint_to_hex_string(uint32_t value, char *result, uint8_t uppercase) {
    const char *hex_chars = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";

    for (int i = 0; i < 8; i++) {
        // Shift by 28, 24, 20, 16, 12, 8, 4, 0 bits
        uint8_t nibble = (value >> ((7 - i) * 4)) & 0x0F;
        result[i] = hex_chars[nibble];
    }
    result[8] = '\0';
}

void print_uint_var(uint32_t var, char* var_name)
{
    char hex[10];
    uint_to_hex_string(var, hex, 1);
    Usart_SendString(DEBUG_USARTx, var_name);
    Usart_SendString(DEBUG_USARTx, ": ");
    Usart_SendString(DEBUG_USARTx, hex);
    Usart_SendString(DEBUG_USARTx, "\n\r");
}

/**
 * @brief  The application entry point.
 *
 * @retval None
 */
int main(void)
{
    /* Initialize systick to 1ms, enable its interrupt */
	SysTick_Init();

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
    uint32_t system_time_start_of_bootloader_catch_ms = systick_get_tick();
    bool bootloader_window_missed = false;
    /* Volatile, because the value is changed by a polling function and read
     * afterwards, an optimizing compiler must not be able to remove the poll. */
    volatile bool flash_request = false;

    // check first flash request, the button push, 500ms timeout
    while (!bootloader_window_missed && !flash_request)
    {
        if (Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
        {
            flash_request = true;
        }
        
        uint32_t system_time_ms = systick_get_tick();

        if (system_time_ms - system_time_start_of_bootloader_catch_ms > BOOTLOADER_WINDOW_MS)
        {
            bootloader_window_missed = true;
        }
    }

    bool bootloader_window2_missed = false;
    system_time_start_of_bootloader_catch_ms = systick_get_tick();
    print_uint_var(system_time_start_of_bootloader_catch_ms, "system_time_start_of_bootloader_catch_ms");
    // check second flash request, the uart msg 'flash', 500ms timeout
    while (!bootloader_window2_missed && !flash_request)
    {
        if (uart_check_contains_flash(DEBUG_USARTx))
        {
            flash_request = true;
        }
        
        uint32_t system_time_ms = systick_get_tick();

        if (system_time_ms - system_time_start_of_bootloader_catch_ms > 10u * BOOTLOADER_WINDOW_MS)
        {
            bootloader_window2_missed = true;
        }
    }
    
    uint32_t system_time_end_of_bootloader_catch_ms = systick_get_tick();
    print_uint_var(system_time_end_of_bootloader_catch_ms, "system_time_end_of_bootloader_catch_ms");
    print_uint_var(flash_request, "flash_request");
    print_uint_var(bootloader_window2_missed, "bootloader_window2_missed");

    if (flash_request)
    {
        Usart_SendString(DEBUG_USARTx, "flash request received...\n\r");
    }
    else
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
