#include "stm32f1xx.h"
#include "simple_module.h"
#include "stm32f1xx_hal_gpio.h"

// LED delay duration
#define LEDDELAY    2000000

int main(){
	
	GPIO_InitTypeDef GPIOB_Params; // Initilisation structure for GPIOB Settings
	
	__HAL_RCC_GPIOB_CLK_ENABLE(); // Turn on Clock of GPIOB
	
	// Configure the GPIO Pins 0, 1, 5 used for LEDs
	GPIOB_Params.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5;
	GPIOB_Params.Mode = GPIO_MODE_OUTPUT_PP; // Set pins to push pull output mode
	GPIOB_Params.Speed = GPIO_SPEED_LOW; // Set low output speed
	HAL_GPIO_Init(GPIOB, &GPIOB_Params); // Initialise GPIOB according to parameters on GPIOB_Params
	
	while(1){
		delay(LEDDELAY); // Delay
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5);
	}
}
