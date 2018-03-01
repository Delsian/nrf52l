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
static uint8_t ubPatternCounter; // Loops
static uint8_t ubPatternPtr;
static uint16_t usTickCounter;

const LedPatternSeq tPatternIdle = {
		.repeats = 0xFF,
		.length = 4,
		.patEnd = NULL,
		.pt = { {0x38, COLOR_CYAN },
				{0x38, COLOR_GREEN },
				{0x38, COLOR_FIREBRICK },
				{0x40, COLOR_MAGENTA } }
};

void RDevLedSetPattern(LedPatternSeq* ipSeq) {
	ubPatternPtr = 0;
	ubPatternCounter = ipSeq->repeats;
	ptPatternSeq = ipSeq;
	usTickCounter = 0x8000; // bigger than max counter value to activate LED init on next tick
}

RDevErrCode RDevLedInit(uint8_t port)
{
	RDevLedSetPattern(&tPatternIdle);
	return RDERR_DONE;
}

RDevErrCode RDevLedCmd(const uint8_t* pData, uint8_t len)
{
	RDevCmdCode ubCommand = (RDevCmdCode)pData[1];
	if(ubCommand == RDCMD_SET) {
		PcaLedColor(pData[2] + pData[3]<<8);
		return RDERR_DONE;
	}
	return RDERR_NOT_SUPPORTED;
}

RDevErrCode RDevLedTick(uint8_t port, uint32_t time)
{
	int i;

	if (ptPatternSeq) {

		usTickCounter++;
		if  ((usTickCounter>>4) > (ptPatternSeq->pt[ubPatternPtr].ticks&0x7F)) {
			if(++ubPatternPtr >=  ptPatternSeq->length) {
				ubPatternPtr = 0;
				if (ubPatternCounter != 0xFF) { // 0xFF if pattern never expires
					if (--ubPatternCounter == 0) {
						if (ptPatternSeq->patEnd) { // Callback on pattern end (restore LED state?)
							// ToDo: Move this to control thread
							(ptPatternSeq->patEnd)(ptPatternSeq);
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
