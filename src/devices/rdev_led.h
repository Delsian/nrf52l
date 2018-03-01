/*
 * rdev_led.h
 *
 *  Created on: 28 лют. 2018 р.
 *      Author: ekrashtan
 */

#ifndef DEVICES_RDEV_LED_H_
#define DEVICES_RDEV_LED_H_

/* RGB565 Color coding (16-bit):
     15..11 = R4..0 (Red)
     10..5  = G5..0 (Green)
      4..0  = B4..0 (Blue)
*/
typedef enum {
	COLOR_BLACK = 0x0000,			/*   0,   0,   0 */
	COLOR_NAVY = 0x000F,			/*   0,   0, 128 */
	COLOR_GREEN = 0x03E0,			/*   0, 128,   0 */
	COLOR_TEAL = 0x03EF,			/*   0, 128, 128 */
	COLOR_MAROON = 0x7800,  		/* 128,   0,   0 */
	COLOR_PURPLE = 0x780F,  		/* 128,   0, 128 */
	COLOR_OLIVE = 0x7BE0,  			/* 128, 128,   0 */
	COLOR_SILVER = 0xC618,  		/* 192, 192, 192 */
	COLOR_GREY = 0x7BEF,  			/* 128, 128, 128 */
	COLOR_BLUE = 0x001F,  			/*   0,   0, 255 */
	COLOR_LIME = 0x07E0, 			/*   0, 255,   0 */
	COLOR_CYAN = 0x07FF,			/*   0, 255, 255 */
	COLOR_RED = 0xF800,				/* 255,   0,   0 */
	COLOR_SALMON = 0xF38F,			/* 250, 114, 128 */
	COLOR_FIREBRICK = 0xA904, 		/* 178,  34,  34 */
	COLOR_MAGENTA = 0xF81F,  		/* 255,   0, 255 */
	COLOR_YELLOW = 0xFFE0,  		/* 255, 255, 0   */
	COLOR_WHITE = 0xFFFF  			/* 255, 255, 255 */
} LedColor;

typedef struct _LedPattern {
	uint8_t ticks; // ticks in x8 intervals
	LedColor color; // color in RGB565 format
} LedPattern;

typedef void (*RDevLedCB)(void* context);

typedef struct {
	uint8_t repeats;
	uint8_t length;
	RDevLedCB patEnd;
	LedPattern pt[];
} LedPatternSeq;

#endif /* DEVICES_RDEV_LED_H_ */
