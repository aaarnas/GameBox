/*
 * buttonFollow.c
 *
 *  Created on: 2018-12-23
 *      Author: aaarn
 */

#include "buttonFollow.h"

#include "buttons.h"
#include "leds.h"
#include "tm1637.h"

#include "cmsis_os.h"

#define SIZE(x) (sizeof(x)/sizeof(x[0]))

enum States {
	STATE_PREPARE_WAIT_START,
	STATE_WAIT_START,
	STATE_SELECT_BUTTON,
	STATE_WAIT_SELECTED,
	STATE_BLOCKED,
};

static volatile int state = STATE_PREPARE_WAIT_START;
static volatile int score = 0;
static volatile int selectedButton = -1;
static volatile int lastSelectedButton = -1;
static volatile int timeleft = -1;
static volatile int timeleftTop = 2000;

static void setTimeleftTop() {
	// Change time
	if (score > 30) {
		if (timeleftTop > 20) timeleftTop--;
	}
	else if (score > 20)
		timeleftTop = 500;
	else if (score > 5)
		timeleftTop = 1000;
	else
		timeleftTop = 2000;
}

static void scoreInc() {
	score++;
	tm1637DisplayDecimal(score, 0);

	setTimeleftTop();
}

static void timeLoop() {

	// Calculate timeout
	if (timeleft == 0) {
		// Fail
		state = STATE_BLOCKED;
		timeleft = -1;

		Led_playXSequence();
		osDelay(500);

		state = STATE_PREPARE_WAIT_START;
	}
	else if (timeleft > 0) {
		timeleft--;
	}
}

static void controlLoop() {

	switch (state) {
		case STATE_PREPARE_WAIT_START: {

			// Play animation
			Led_playSpiralSequence();

			// Light white button
			Led_set(WHITE_BUTTON, 1);

			state++; break;
		}
		case STATE_WAIT_START: {

			if (Button_getPressed() == WHITE_BUTTON) {

				Button_updateRandom();
				Led_set(WHITE_BUTTON, 0);
				// Reset score
				score = 0;
				tm1637DisplayDecimal(score, 0);
				osDelay(20);
				selectedButton = WHITE_BUTTON;
				lastSelectedButton = WHITE_BUTTON;
				setTimeleftTop();
				state++;
			}
			break;
		}
		case STATE_SELECT_BUTTON: {

			selectedButton = Button_getRandom(selectedButton);
			Led_set(selectedButton, 1);
			timeleft = timeleftTop;

			state++; break;
		}
		case STATE_WAIT_SELECTED: {

			int button = Button_getPressed();
			if (button != -1) {

				if (button == lastSelectedButton) { /** skip **/ }
				else if (button == selectedButton) {
					// Button correct. Switch to another
					lastSelectedButton = button;
					Led_set(selectedButton, 0);

					// Increase score
					scoreInc();

					// Prepare new button
					state = STATE_SELECT_BUTTON;
					osDelay(20); //Debounce
				}
				else {
					// Fail
					timeleft = -1;

					Led_playXSequence();
					osDelay(500);

					state = STATE_PREPARE_WAIT_START;
				}
			}
			else {
				// No button pressing. Reset last selected button
				lastSelectedButton = -1;
			}
			break;
		}
		case STATE_BLOCKED: {
			// Do nothing
			break;
		}
	}
}

void Game_buttonFollow(void (**timeTask)(), void (**controlTask)()) {

	state = STATE_PREPARE_WAIT_START;

	tm1637DisplayDecimal(0, 0);

	*timeTask = timeLoop;
	*controlTask = controlLoop;
}
