/*
 * buttonMemory.c
 *
 *  Created on: 2018-12-23
 *      Author: aaarn
 */

#include "buttonMemory.h"

#include "buttons.h"
#include "leds.h"
#include "tm1637.h"

#include "cmsis_os.h"
#include "SEGGER_RTT.h"
#define SIZE(x) (sizeof(x)/sizeof(x[0]))

static uint8_t sequence[50];
static int sequenceLen = 0;

static void generateSequence(uint8_t seq[], int buttonsCnt) {

	seq[0] = Button_getRandom(-1);
	for(int i=1; i<buttonsCnt; i++) {
		seq[i] = Button_getRandom(seq[i-1]);
	}
}

enum States {
	STATE_PREPARE_WAIT_START,
	STATE_WAIT_START,
	STATE_SHOW_SEQUENCE,
	STATE_COLLECT_SEQUENCE,
};

static volatile int state = STATE_PREPARE_WAIT_START;
static volatile int sequenceCollectIndex = 0;
static volatile int lastButtonPressed = -1;
static volatile int score = 0;

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
			osDelay(1000);
			state++;
		}
		break;
	}
	case STATE_SHOW_SEQUENCE: {

		sequenceLen = score/2;
		if (sequenceLen < 2) sequenceLen = 2;
		generateSequence(sequence, sequenceLen);
		sequenceCollectIndex = 0;
		Led_playSequence(sequence, sequenceLen, 500);
		lastButtonPressed = -1;

		state++; break;
	}
	case STATE_COLLECT_SEQUENCE: {

		int button = Button_getPressed();
		if (button != -1) {

			if (button == lastButtonPressed) { /** skip **/ }
			else if (button == sequence[sequenceCollectIndex]) {

				if (sequenceCollectIndex > 0)
					Led_set(sequence[sequenceCollectIndex-1], 0);
				Led_set(sequence[sequenceCollectIndex], 1);

				sequenceCollectIndex++;

				if (sequenceCollectIndex == sequenceLen) {
					osDelay(200);
					Led_offAll();
					osDelay(300);
					state = STATE_SHOW_SEQUENCE;
					score++;
					tm1637DisplayDecimal(score, 0);
					osDelay(500);
				}

				lastButtonPressed = button;

				osDelay(20); // Debouce
			}
			else {
				// Fail
				Led_playXSequence();
				osDelay(500);

				state = STATE_PREPARE_WAIT_START;
			}
		}
		break;
	}
	}
}

void Game_buttonMemory(void (**timeTask)(), void (**controlTask)()) {

	state = STATE_PREPARE_WAIT_START;

	tm1637DisplayDecimal(0, 0);

	*timeTask = (void*)0;
	*controlTask = controlLoop;
}
