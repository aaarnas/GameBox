/*
 * leds.c
 *
 *  Created on: 2018-12-22
 *      Author: aaarn
 */

#include "leds.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#define SIZE(x) (sizeof(x)/sizeof(x[0]))
typedef struct {
	GPIO_TypeDef *port;
	uint32_t      pin;
} Led;

static Led leds[] = {
		{GPIOB, GPIO_PIN_14},  // Led 0
		{GPIOA, GPIO_PIN_12},  // Led 1
		{GPIOA, GPIO_PIN_8},   // Led 2
		{GPIOB, GPIO_PIN_13},  // Led 3
		{GPIOA, GPIO_PIN_11},  // Led 4
		{GPIOA, GPIO_PIN_9},   // Led 5
		{GPIOB, GPIO_PIN_12},  // Led 6
		{GPIOA, GPIO_PIN_10},  // Led 7
		{GPIOB, GPIO_PIN_15},  // Led 8
};

void Led_init() {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	for (int b=0; b<SIZE(leds); b++) {
		// Turn off all leds
		HAL_GPIO_WritePin(leds[b].port, leds[b].pin, GPIO_PIN_SET);
		// Initialize all leds
		GPIO_InitStruct.Pin = leds[b].pin;
		HAL_GPIO_Init(leds[b].port, &GPIO_InitStruct);
	}
}

void Led_set(int index, int state) {
	if (index < 0 || index >= SIZE(leds)) return;
	HAL_GPIO_WritePin(leds[index].port, leds[index].pin, state ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

uint8_t patternX[SIZE(leds)] = {
		1, 0, 1,
		0, 1, 0,
		1, 0, 1
};

uint8_t patternOff[SIZE(leds)] = {
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
};

static void setPattern(uint8_t pattern[]) {
	for (int i=0; i<SIZE(leds); i++) {
		Led_set(i, pattern[i]);
	}
}

void Led_offAll() {
	setPattern(patternOff);
}

void Led_playSequence(uint8_t seq[], int buttonsCnt, int delay) {
	Led_offAll();
	for (int i=0; i<buttonsCnt; i++) {
		Led_set(seq[i], 1);
		osDelay(delay);
		Led_set(seq[i], 0);
	}
}

uint8_t sequenceSpiral[] = {0, 1, 2, 5, 8, 7, 6, 3, 4};

void Led_playSpiralSequence() {
	Led_playSequence(sequenceSpiral, sizeof(sequenceSpiral), 50);
}

void Led_playXSequence() {
	for (int i=0; i<5; i++) {
		setPattern(patternX);
		osDelay(100);
		setPattern(patternOff);
		osDelay(100);
	}
}

