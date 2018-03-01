/*
 * rdev_led.h
 *
 *  Created on: 28 лют. 2018 р.
 *      Author: ekrashtan
 */

#ifndef DEVICES_RDEV_LED_H_
#define DEVICES_RDEV_LED_H_

typedef struct _LedPattern {
	uint8_t ticks; // ticks in x10 intervals, bit 7 indicates fade-in/fade-out
	uint8_t color[3];
} LedPattern;

typedef void (*RDevLedCB)(void* context);

typedef struct {
	uint8_t repeats;
	uint8_t length;
	RDevLedCB patEnd;
	LedPattern pt[];
} LedPatternSeq;

#endif /* DEVICES_RDEV_LED_H_ */
