#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "Motor.h"
#include "key.h"

uint8_t Address;
uint8_t Command;
uint8_t Num;
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	Remote_Init(30000,72);
	Motor_Init();
	Key_Init();
	while(1)
	{
		Num = Key_GetNum();
		if (Num == 1)
		{
			Car_TurnLeft(500);
		}
		else if (Num == 2)
		{
			Car_TurnRight(500);
		}
		else if (Num == 3)
		{
			Car_TransLeft(500);
		}
		else if (Num == 4)
		{
			Car_TransRight(500);
		}		
	}
}


