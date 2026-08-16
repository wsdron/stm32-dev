/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   串口中断接收测试
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
 
 
#include "stm32f10x.h"
#include "./led/bsp_led.h" 
#include "./usart/bsp_usart.h" 

static void Show_Message(void);
  
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
  char ch;
  
  /* 初始化RGB彩灯 */
  LED_GPIO_Config();
  
  /* 初始化USART 配置模式为 115200 8-N-1 */
  USART_Config();
	
  /* 打印指令输入提示信息 */
  Show_Message();
  while(1)
	{	
    /* 获取字符指令 */
    ch=usart_get_char();
	Usart_SendString( DEBUG_USARTx,"command received:\n");
    Usart_SendByte( DEBUG_USARTx, ch);
    
    /* 根据字符指令控制RGB彩灯颜色 */
    switch(ch)
    {
      case '1':
        LED_RED;
      break;
      case '2':
        LED_GREEN;
      break;
      case '3':
        LED_BLUE;
      break;
      case '4':
        LED_YELLOW;
      break;
      case '5':
        LED_PURPLE;
      break;
      case '6':
        LED_CYAN;
      break;
      case '7':
        LED_WHITE;
      break;
      case '8':
        LED_RGBOFF;
      break;
      default:
        /* 如果不是指定指令字符，打印提示信息 */
        Show_Message();
        break;      
    }   
	}	
}


/**
  * @brief  打印指令输入提示信息
  * @param  无
  * @retval 无
  */
static void Show_Message(void)
{
  Usart_SendString(DEBUG_USARTx,"\r\n   simple led control experiment \n");
  Usart_SendString(DEBUG_USARTx,"USART config: 8-N-1, baud rate: \n");
  Usart_SendString(DEBUG_USARTx,"   command   ------ color\n");
  Usart_SendString(DEBUG_USARTx,"     1    ------    red \n");
  Usart_SendString(DEBUG_USARTx,"     2    ------    green \n");
  Usart_SendString(DEBUG_USARTx,"     3    ------    blue \n");
  Usart_SendString(DEBUG_USARTx,"     4    ------    yellow \n");
  Usart_SendString(DEBUG_USARTx,"     5    ------    purple \n");
  Usart_SendString(DEBUG_USARTx,"     6    ------    green \n");
  Usart_SendString(DEBUG_USARTx,"     7    ------    white \n");
  Usart_SendString(DEBUG_USARTx,"     8    ------    off \n");  
}

/*********************************************END OF FILE**********************/
