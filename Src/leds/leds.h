/*
 * leds.h
 *
 *  Created on: 2018-12-22
 *      Author: aaarn
 */

#ifndef SRC_LEDS_LEDS_H_
#define SRC_LEDS_LEDS_H_

#include <stdint.h>

void Led_init();
void Led_set(int index, int state);
void Led_offAll();
void Led_playSequence(uint8_t seq[], int buttonsCnt, int delay);
void Led_playSpiralSequence();
void Led_playXSequence();

#endif /* SRC_LEDS_LEDS_H_ */
