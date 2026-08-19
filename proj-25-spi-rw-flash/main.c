 /**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   华邦 8M串行flash测试，并将测试信息通过串口1在电脑的超级终端中打印出来
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
#include "./usart/bsp_usart.h"
#include "./led/bsp_led.h"
#include "./flash/bsp_spi_flash.h"
#include <stdint.h>
#include <string.h>

typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

/* 获取缓冲区的长度 */
#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define RxBufferSize1   (countof(TxBuffer1) - 1)
#define countof(a)      (sizeof(a) / sizeof(*(a)))
#define  BufferSize (countof(Tx_Buffer)-1)
#define  buffer_size (countof(Tx_Buffer))

#define  FLASH_WriteAddress     0x00000
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress

     

/* 发送缓冲区初始化 */
uint8_t Tx_Buffer[] = "thank you for using embed fire board\r\n";
uint8_t Rx_Buffer[BufferSize];

char tx_buffer[buffer_size];
char rx_buffer[buffer_size];

__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;
__IO TestStatus TransferStatus1 = FAILED;

// 函数原型声明
void Delay(__IO uint32_t nCount);
TestStatus Buffercmp(uint8_t* pBuffer1,uint8_t* pBuffer2, uint16_t BufferLength);

// helper function to convert uint32 to string
void uint32_to_hex_string(uint32_t val, char *out_str) {
    // Lookup table for 4-bit values (0x0 to 0xF)
    static const char hex_digits[] = "0123456789ABCDEF";

    // Process 8 nibbles (4 bits each), starting from the highest bit
    for (int i = 7; i >= 0; i--) {
        out_str[7 - i] = hex_digits[(val >> (i * 4)) & 0x0F];
    }
}

void convert_text_bytes(const uint8_t *src, size_t len, char *dest) {
    // Copy bytes directly
    memcpy(dest, src, len);
}

/*
 * 函数名：main
 * 描述  ：主函数
 * 输入  ：无
 * 输出  ：无
 */
int main(void)
{ 	
	LED_GPIO_Config();
	LED_BLUE;
	
	/* 配置串口为：115200 8-N-1 */
	USART_Config();
	Usart_SendString( DEBUG_USARTx,"this is spi read/write flash w25q64 experiment\n");

	/* 8M串行flash W25Q64初始化 */
	SPI_FLASH_Init();
	
	/* 获取 Flash Device ID */
	DeviceID = SPI_FLASH_ReadDeviceID();	
	Delay( 200 );
	
	/* 获取 SPI Flash ID */
	FlashID = SPI_FLASH_ReadID();	
    
    char buff_id[8];
    uint32_to_hex_string(FlashID, buff_id); 
	Usart_SendString( DEBUG_USARTx,"flash id is:\n");
	Usart_SendString( DEBUG_USARTx, buff_id);
	Usart_SendString( DEBUG_USARTx,"\n");

    uint32_to_hex_string(DeviceID, buff_id); 
	Usart_SendString( DEBUG_USARTx,"device id is:\n");
	Usart_SendString( DEBUG_USARTx, buff_id);
	Usart_SendString( DEBUG_USARTx,"\n");
        
	/* 检验 SPI Flash ID */
	if (FlashID == sFLASH_ID)
	{	
	    Usart_SendString( DEBUG_USARTx,"detected flash w25q64\n");
		
		/* 擦除将要写入的 SPI FLASH 扇区，FLASH写入前要先擦除 */
		// 这里擦除4K，即一个扇区，擦除的最小单位是扇区
		SPI_FLASH_SectorErase(FLASH_SectorToErase);	 	 
		
		/* 将发送缓冲区的数据写到flash中 */
		// 这里写一页，一页的大小为256个字节
		SPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize);		
	    Usart_SendString( DEBUG_USARTx,"data going to be written\n");
        convert_text_bytes(Tx_Buffer, buffer_size, tx_buffer);
	    Usart_SendString( DEBUG_USARTx, tx_buffer);
		
		/* 将刚刚写入的数据读出来放到接收缓冲区中 */
		SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);
	    Usart_SendString( DEBUG_USARTx,"data read:\n");
        convert_text_bytes(Rx_Buffer, buffer_size, rx_buffer);
	    Usart_SendString( DEBUG_USARTx, rx_buffer);
		
		/* 检查写入的数据与读出的数据是否相等 */
		TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);
		
		if( PASSED == TransferStatus1 )
		{ 
			LED_GREEN;
	        Usart_SendString( DEBUG_USARTx, "experiment succeeded! \n");
		}
		else
		{        
			LED_RED;
	        Usart_SendString( DEBUG_USARTx, "experiment failed... \n");
		}
	}// if (FlashID == sFLASH_ID)
	else// if (FlashID == sFLASH_ID)
	{ 
		LED_RED;
	    Usart_SendString( DEBUG_USARTx,"did not detect flash w25q64\n");
	}
	
	while(1);  
}

/*
 * 函数名：Buffercmp
 * 描述  ：比较两个缓冲区中的数据是否相等
 * 输入  ：-pBuffer1     src缓冲区指针
 *         -pBuffer2     dst缓冲区指针
 *         -BufferLength 缓冲区长度
 * 输出  ：无
 * 返回  ：-PASSED pBuffer1 等于   pBuffer2
 *         -FAILED pBuffer1 不同于 pBuffer2
 */
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }
  return PASSED;
}

void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
