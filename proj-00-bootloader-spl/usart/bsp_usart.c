/**
 ******************************************************************************
 * @file    bsp_usart.c
 * @author  fire
 * @version V1.0
 * @date    2013-xx-xx
 * @brief   重定向c库printf函数到usart端口
 ******************************************************************************
 * @attention
 *
 * 实验平台:野火STM32 F103-指南者 开发板
 * 论坛    :http://www.firebbs.cn
 * 淘宝    :https://fire-stm32.taobao.com
 *
 ******************************************************************************
 */

#include "bsp_usart.h"
#include "bsp_SysTick.h"

/**
 * @brief  配置嵌套向量中断控制器NVIC
 * @param  无
 * @retval 无
 */
#if 0
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 嵌套向量中断控制器组选择 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* 配置USART为中断源 */
    NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;
    /* 抢断优先级*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    /* 子优先级 */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    /* 使能中断 */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /* 初始化配置NVIC */
    NVIC_Init(&NVIC_InitStructure);
}
#endif

/**
 * @brief  USART GPIO 配置,工作参数配置
 * @param  无
 * @retval 无
 */
void USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // 打开串口GPIO的时钟
    DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);

    // 打开串口外设的时钟
    DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);

    // 将USART Tx的GPIO配置为推挽复用模式
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

    // 将USART Rx的GPIO配置为浮空输入模式
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);

    // 配置串口的工作参数
    // 配置波特率
    USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
    // 配置 针数据字长
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    // 配置停止位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    // 配置校验位
    USART_InitStructure.USART_Parity = USART_Parity_No;
    // 配置硬件流控制
    USART_InitStructure.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;
    // 配置工作模式，收发一起
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    // 完成串口的初始化配置
    USART_Init(DEBUG_USARTx, &USART_InitStructure);

    /*
        // 串口中断优先级配置
        NVIC_Configuration();

        // 使能串口接收中断
        USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);
        */

    // 使能串口
    USART_Cmd(DEBUG_USARTx, ENABLE);
}

/*****************  发送一个字节 **********************/
void Usart_SendByte(USART_TypeDef *pUSARTx, uint8_t ch)
{
    /* 发送一个字节数据到USART */
    USART_SendData(pUSARTx, ch);

    /* 等待发送数据寄存器为空 */
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET)
        ;
}

/****************** 发送8位的数组 ************************/
void Usart_SendArray(USART_TypeDef *pUSARTx, uint8_t *array, uint16_t num)
{
    uint8_t i;

    for (i = 0; i < num; i++)
    {
        /* 发送一个字节数据到USART */
        Usart_SendByte(pUSARTx, array[i]);
    }
    /* 等待发送完成 */
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET)
        ;
}

/*****************  发送字符串 **********************/
void Usart_SendString(USART_TypeDef *pUSARTx, char *str)
{
    unsigned int k = 0;
    do
    {
        Usart_SendByte(pUSARTx, *(str + k));
        k++;
    } while (*(str + k) != '\0');

    /* 等待发送完成 */
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET)
    {
    }
}

/*****************  发送一个16位数 **********************/
void Usart_SendHalfWord(USART_TypeDef *pUSARTx, uint16_t ch)
{
    uint8_t temp_h, temp_l;

    /* 取出高八位 */
    temp_h = (ch & 0XFF00) >> 8;
    /* 取出低八位 */
    temp_l = ch & 0XFF;

    /* 发送高八位 */
    USART_SendData(pUSARTx, temp_h);
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET)
        ;

    /* 发送低八位 */
    USART_SendData(pUSARTx, temp_l);
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET)
        ;
}

uart_status uart_receive(USART_TypeDef *pUSART, uint8_t *data, uint16_t length)
{
    uart_status status = UART_OK;
    uint32_t start_tick = systick_get_tick();

    for (uint16_t i = 0; i < length; i++)
    {
        // Wait until the RX Buffer Not Empty (RXNE) flag is set
        // or 1ms time out
        while (RESET == USART_GetFlagStatus(pUSART, USART_FLAG_RXNE))
        {
            if (systick_get_tick() - start_tick > 1u)
            {
                return UART_ERROR; // or UART_ERROR based on your enum
            }
        }

        // Read received byte (clears the RXNE flag automatically)
        data[i] = (uint8_t)USART_ReceiveData(USART1);
    }

    return status;
}

/**
 * @brief  Collects up to 30 characters within 1ms and checks if 'flash' is present.
 * @param  pUSARTx: Selected USART peripheral (e.g., USART1, USART2).
 * @retval true if 'flash' is found within the received window, false otherwise.
 */
bool uart_check_contains_flash(USART_TypeDef *pUSARTx) {
    uint8_t buffer[30] = {0};
    uint32_t count = 0u;
    uint32_t start_tick = systick_get_tick();
    bool return_val = false;

    // Collect up to 30 characters or until 1 ms passes
    while ((count < 30u) && ((systick_get_tick() - start_tick) < 1u)) {
        uint8_t rx_byte = 0;
        
        // Attempt to read a single byte (non-blocking or short poll inside uart_receive)
        if (uart_receive(pUSARTx, &rx_byte, 1) == UART_OK) {
            buffer[count++] = rx_byte;
        }
    }

    // Search the collected bytes for the substring "flash"
    if (count >= 5) {
        for (uint32_t i = 0; i <= (count - 5); i++) {
            if (buffer[i]     == 'f' &&
                buffer[i + 1] == 'l' &&
                buffer[i + 2] == 'a' &&
                buffer[i + 3] == 's' &&
                buffer[i + 4] == 'h') {
                return_val = true;
            }
        }
    }

    return return_val;
}

/*
///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
                // 发送一个字节数据到串口
                USART_SendData(DEBUG_USARTx, (uint8_t) ch);

                // 等待发送完毕
                while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) ==
RESET);

                return (ch);
}

///重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
                // 等待串口输入数据
                while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) ==
RESET);

                return (int)USART_ReceiveData(DEBUG_USARTx);
}
*/
