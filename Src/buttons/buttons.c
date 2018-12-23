/*
 * buttons.c
 *
 *  Created on: 2018-12-22
 *      Author: aaarn
 */

#include <stdlib.h>
#include "buttons.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#define SIZE(x) (sizeof(x)/sizeof(x[0]))
typedef struct {
	GPIO_TypeDef *port;
	uint32_t      pin;
} Button;

static Button buttons[] = {
		{GPIOB, GPIO_PIN_11}, // Button 0
		{GPIOB, GPIO_PIN_0},  // Button 1
		{GPIOA, GPIO_PIN_3},  // Button 2
		{GPIOB, GPIO_PIN_10}, // Button 3
		{GPIOA, GPIO_PIN_6},  // Button 4
		{GPIOA, GPIO_PIN_4},  // Button 5
		{GPIOB, GPIO_PIN_1},  // Button 6
		{GPIOA, GPIO_PIN_7},  // Button 7
		{GPIOA, GPIO_PIN_5},  // Button 8
};

void Button_init() {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	for (int b=0; b<SIZE(buttons); b++) {
		GPIO_InitStruct.Pin = buttons[b].pin;
		HAL_GPIO_Init(buttons[b].port, &GPIO_InitStruct);
	}
}

int Button_getPressed() {

	for (int b=0; b<SIZE(buttons); b++) {
		if (HAL_GPIO_ReadPin(buttons[b].port, buttons[b].pin) == GPIO_PIN_RESET)
			return b;
	}

	return -1;
}

int Button_isPressed(int index) {
	if (index < 0 || index >= SIZE(buttons)) return 0;

	return HAL_GPIO_ReadPin(buttons[index].port, buttons[index].pin) == GPIO_PIN_RESET;
}

int Button_getRandom(int btnToIgnore) {
	int newButton;
	while ((newButton = rand() % 8) == btnToIgnore);
	return newButton;
}

void Button_updateRandom() {
	srand(osKernelSysTick());
}
