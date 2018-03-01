/*
 * rdev_led.c
 *
 *  Created on: Feb 18, 2018
 *      Author: Eug
 */

#include <stdint.h>
#include "pca9685.h"
#include "boards.h"
#include "r0b1c_device.h"
#include "r0b1c_cmd.h"
#include "rdev_led.h"

static LedPatternSeq* ptPatternSeq;
static uint8_t ubPatternCounter; // Loops. If pattern counter = 0xFF - continuous
static uint8_t ubPatternPtr;
static uint16_t usTickCounter;
static LedColor tExternalColor;

const LedPatternSeq tPatternIdle = {
		.repeats = 0xFF,
		.length = 4,
		.patEnd = NULL,
		.pt = { {0x38, COLOR_CYAN },
				{0x38, COLOR_GREEN },
				{0x38, COLOR_FIREBRICK },
				{0x40, COLOR_MAGENTA } }
};

LedPatternSeq* RDevLedSetPattern(LedPatternSeq* ipSeq) {
	LedPatternSeq* oldP = ptPatternSeq;
	ubPatternPtr = 0;
	ubPatternCounter = ipSeq->repeats;
	ptPatternSeq = ipSeq;
	usTickCounter = 0x8000; // bigger than max counter value to activate LED init on next tick
	return oldP;
}

RDevErrCode RDevLedInit(uint8_t port)
{
	RDevLedSetPattern(&tPatternIdle);
	return RDERR_DONE;
}

RDevErrCode RDevLedCmd(const uint8_t* pData, uint8_t len)
{
	RDevCmdCode ubCommand = (RDevCmdCode)pData[1];
	switch (ubCommand) {
	case RDCMD_SET:
		tExternalColor = pData[2] + ((uint16_t)pData[3])<<8;
		// Turn off pattern blinking
		ubPatternPtr = 0xFF;
		PcaLedColor(tExternalColor);
		return RDERR_DONE;
	case RDCMD_RESET:
		tExternalColor = COLOR_BLACK;
		if (ptPatternSeq)
			ubPatternPtr = 0; // Restart pattern
		return RDERR_DONE;
	default:
		break;
	}
	return RDERR_NOT_SUPPORTED;
}

RDevErrCode RDevLedTick(uint8_t port, uint32_t time)
{
	int i;
	if (ubPatternPtr == 0xff)
		return RDERR_DONE; // fixed color

	if (ptPatternSeq) {
		usTickCounter++;
		if  ((usTickCounter>>4) > (ptPatternSeq->pt[ubPatternPtr].ticks)) {
			if(++ubPatternPtr >=  ptPatternSeq->length) {
				ubPatternPtr = 0;
				if (ubPatternCounter != 0xFF) { // 0xFF if pattern never expires
					if (--ubPatternCounter == 0) {
						if (ptPatternSeq->patEnd) { // Callback on pattern end (restore LED state?)
							// ToDo: Move this to control thread
							(ptPatternSeq->patEnd)(ptPatternSeq);
						} else {
							PcaLedColor(tExternalColor);
						}
						ptPatternSeq = NULL;

						return RDERR_DONE;
					}
				}
			}
			usTickCounter = 0;
			PcaLedColor(ptPatternSeq->pt[ubPatternPtr].color);
		}
	}

	return RDERR_DONE;
}
