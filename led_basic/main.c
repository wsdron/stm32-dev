#include "stm32f10x.h"

// LED delay duration
#define LEDDELAY    2000000

int main(){
	
	GPIO_InitTypeDef GPIOB_Params; // Initilisation structure for GPIOB Settings
	
	__HAL_RCC_GPIOB_CLK_ENABLE(); // Turn on Clock of GPIOB
	
	// Configure the GPIO Pins 0, 1, 5 used for LEDs
	GPIOB_Params.Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5;
	GPIOB_Params.Mode = GPIO_Mode_Out_PP; // Set Pins to push pull output mode
	GPIOB_Params.Speed = GPIO_SPEED_LOW; // Set low output speed
	GPIO_Init(GPIOB, &GPIOB_Params); // Initialise GPIOB according to parameters on GPIOB_Params
	
	while(1){
		delay(LEDDELAY); // Delay
		HAL_GPIO_TogglePin(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5);
	}
}
