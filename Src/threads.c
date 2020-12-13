/*
 * threads.c
 *
 *  Created on: 2018-12-22
 *      Author: aaarn
 */

#include "leds.h"
#include "buttons.h"
#include "tm1637.h"

#include "buttonFollow.h"
#include "buttonMemory.h"

#define GAME_BUTTON_FLOW   0
#define GAME_BUTTON_MEMORY 1

#define SHUTDOWN_COUNTER 60
#define SLEEP_DELAY 60000

#define DISPLAY_BRIGHTNESS 5

void (*timeFunc)() = NULL;
void (*controlsFunc)() = NULL;

static uint8_t wordSEL[4] = {0x0, 0x38, 0x79, 0x6d};
static uint8_t game1[4] = {0x06, 0x0, 0x0, 0x0};
static uint8_t game2[4] = {0x5b, 0x0, 0x0, 0x0};
static uint8_t dnull[4] = {0x0, 0x0, 0x0, 0x0};
static uint32_t menuButtonHoldTime = 0;
static int shutdownCounter = 0;
static uint32_t sleepTime = 0;
static int isSleepMode = 0;
static int isMenuActive = 0;

void init() {

	Led_init();
	Button_init();
	tm1637Init();
	tm1637SetBrightness(DISPLAY_BRIGHTNESS);
	tm1637DisplayDecimal(0, 0);

	Game_buttonFollow(&timeFunc, &controlsFunc);
//	Game_buttonMemory(&timeFunc, &controlsFunc);
}

void showMenu() {
	timeFunc = NULL;
	controlsFunc = NULL;

	Led_offAll();
	Led_set(GAME_BUTTON_FLOW, 1);
	Led_set(GAME_BUTTON_MEMORY, 1);

	tm1637Display(wordSEL, 0);

	isMenuActive = 1;
}

void selectMenu() {
	Led_offAll();
	Led_set(GAME_BUTTON_FLOW, 1);
	Led_set(GAME_BUTTON_MEMORY, 1);

	if (Button_isPressed(GAME_BUTTON_FLOW)) {
		Led_set(GAME_BUTTON_FLOW, 0);
		tm1637Display(game1, 0);
		for (int i=0; i<3; i++) {
			tm1637SetBrightness(0);
			osDelay(200);
			tm1637SetBrightness(DISPLAY_BRIGHTNESS);
			osDelay(200);
		}
		isMenuActive = 0;
		Game_buttonFollow(&timeFunc, &controlsFunc);
	}
	else if (Button_isPressed(GAME_BUTTON_MEMORY)) {
		Led_set(GAME_BUTTON_MEMORY, 0);
		tm1637Display(game2, 0);
		for (int i=0; i<3; i++) {
			tm1637SetBrightness(0);
			osDelay(200);
			tm1637SetBrightness(DISPLAY_BRIGHTNESS);
			osDelay(200);
		}
		isMenuActive = 0;
		Game_buttonMemory(&timeFunc, &controlsFunc);
	}
}

void ShutdownMonitorThread(void const * argument)
{
	for (;;)
	{
		if (isSleepMode) {
			// Blink dots
			tm1637SetBrightness(DISPLAY_BRIGHTNESS);
			osDelay(70);
			tm1637SetBrightness(0);
			if (shutdownCounter-- < 0) {
			    // Put to lowest power mode
			    HAL_PWR_EnterSTANDBYMode();
			}
		}
		else if (sleepTime) {
			if (sleepTime < osKernelSysTick()) {
				// Enter sleep
				timeFunc = NULL;
				controlsFunc = NULL;
				Led_offAll();
				tm1637Display(dnull, 1);
				tm1637SetBrightness(0);
				isSleepMode = 1;
				sleepTime = 0;
				shutdownCounter = SHUTDOWN_COUNTER;
			}
		}

		osDelay(1000);
	}
}

void TimeThread(void const * argument)
{
	for(;;)
	{
		if (timeFunc) timeFunc();
		osDelay(1);
	}
}

void ControlsThread(void const * argument)
{
	for(;;)
	{
		if (isSleepMode) {
			if (Button_getPressed() != -1) {
				isSleepMode = 0;
				tm1637SetBrightness(DISPLAY_BRIGHTNESS);
				Game_buttonFollow(&timeFunc, &controlsFunc);
			}
			continue;
		}

		if (isMenuActive) {
			selectMenu();
			osDelay(1);
			continue;
		}

		if (controlsFunc) controlsFunc();
		osDelay(1);

		if (Button_isPressed(WHITE_BUTTON)) {
			if (menuButtonHoldTime == 0) {
				menuButtonHoldTime = osKernelSysTick()+3000;
			}
			else if (menuButtonHoldTime < osKernelSysTick()) {
				showMenu();
			}
		}
		else menuButtonHoldTime = 0;

		static uint8_t lastBtn, btn;
		btn = Button_getPressed();
		if (btn != -1) {
			if (btn != lastBtn) {
				sleepTime = osKernelSysTick()+SLEEP_DELAY;
				lastBtn = btn;
			}
		}
		else {
			lastBtn = -1;
		}
	}
}
