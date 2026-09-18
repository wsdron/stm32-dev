#ifndef _USART_H_
#define _USART_H_

#include <stdio.h>
#include <stdint.h>

extern char Rx_Packet[100];
void Usart1_Init(void);
void Usart1_SendByte(uint8_t Byte);
uint8_t Usart1_GetRxFlag(void);
uint8_t Usart1_GetRxData(void);
void Usart1_SendArray(uint8_t *Array, uint16_t Length);
void Usart1_SendString(char *String);
void Usart1_SendNum(uint32_t Number, uint8_t Length);
void Usaert1_SendPacket(uint8_t *Array, uint16_t Length);
#endif
