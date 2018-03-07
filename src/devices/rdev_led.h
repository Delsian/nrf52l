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
	COLOR_BLUE = 0x001F,  			/*   0,   0, 255 */
	COLOR_GREEN = 0x03E0,			/*   0, 128,   0 */
	COLOR_TEAL = 0x03EF,			/*   0, 128, 128 */
	COLOR_LIME = 0x07E0, 			/*   0, 255,   0 */
	COLOR_CYAN = 0x07FF,			/*   0, 255, 255 */
	COLOR_TURQUOISE = 0x3E7B,		/*  64, 208, 224 */
	COLOR_MAROON = 0x7800,  		/* 128,   0,   0 */
	COLOR_PURPLE = 0x780F,  		/* 128,   0, 128 */
	COLOR_OLIVE = 0x7BE0,  			/* 128, 128,   0 */
	COLOR_GREY = 0x7BEF,  			/* 128, 128, 128 */
	COLOR_YELLOW_GREEN = 0x9198,	/* 154,  50, 205 */
	COLOR_FIREBRICK = 0xA904, 		/* 178,  34,  34 */
	COLOR_SILVER = 0xC618,  		/* 192, 192, 192 */
	COLOR_SALMON = 0xF38F,			/* 250, 114, 128 */
	COLOR_RED = 0xF800,				/* 255,   0,   0 */
	COLOR_ORANGE = 0xF814,			/* 255,   0, 165 */
	COLOR_MAGENTA = 0xF81F,  		/* 255,   0, 255 */
	COLOR_YELLOW = 0xFFE0,  		/* 255, 255, 0   */
	COLOR_WHITE = 0xFFFF  			/* 255, 255, 255 */
} LedColor;

typedef enum {
	LED_IND_NONE 		= 0x0000,
	LED_IND_IDLE 		= 0x0001,
	LED_IND_BTCONN 		= 0x0002, // Bluetooth connected
	LED_IND_CHARGING 	= 0x0004,
	LED_IND_CHARGED 	= 0x0008,
	LED_IND_LOWBATT 	= 0x0010,

	LED_IND_RUNNING		= 0x4000,
	LED_IND_EXTCOLOR 	= 0x8000 // Maximum priority for external color set
} LedIndication;

typedef struct _LedPattern {
	uint8_t ticks; // ticks in x8 intervals
	LedColor color; // color in RGB565 format
} LedPattern;

typedef struct {
	LedIndication type;
	uint8_t length;
	LedPattern pt[4]; // Max pattern length, enough to implement 2 short blinks
} LedPatternSeq;

void RDevLedSetIndication(LedIndication ind);
void RDevLedClearIndication(LedIndication ind);
void RDevLedSetColor(LedColor c);

#endif /* DEVICES_RDEV_LED_H_ */
