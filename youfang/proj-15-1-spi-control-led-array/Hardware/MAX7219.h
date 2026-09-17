#ifndef _MAX7219_H_
#define _MAX7219_H_

void MAX7219_Init(void);
void Write_MAX7219(uint8_t addr, uint8_t data);
void MAX7219_CS_Init(void);
#endif
