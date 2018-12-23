/*
 * tm1637.h
 *
 *  Created on: 2018-12-22
 *      Author: aaarn
 */

#ifndef SRC_TM1637_TM1637_H_
#define SRC_TM1637_TM1637_H_

void tm1637Demo(void);
void tm1637Init(void);
void tm1637Display(unsigned char digitArr[4], int displaySeparator);
void tm1637DisplayDecimal(int v, int displaySeparator);
void tm1637SetBrightness(char brightness);

#endif /* SRC_TM1637_TM1637_H_ */
