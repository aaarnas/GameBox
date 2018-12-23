/*
 * buttons.h
 *
 *  Created on: 2018-12-22
 *      Author: aaarn
 */

#ifndef SRC_BUTTONS_BUTTONS_H_
#define SRC_BUTTONS_BUTTONS_H_

#define WHITE_BUTTON 4

void Button_init();
int Button_getPressed();
int Button_isPressed(int index);
int Button_getRandom(int btnToIgnore);

void Button_updateRandom();

#endif /* SRC_BUTTONS_BUTTONS_H_ */
