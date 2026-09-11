#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "PWM.h"
#include "key.h"

int main(void)
{
	PWM_Init();

	while(1)
	{
		PWM_SetCompare3(0);
		PWM_SetCompare4(1000);
	}
}


