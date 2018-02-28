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
		.length = 2,
		.patEnd = NULL,
		.pt = {{20, 0xFF, 0, 0 },
		{10, 0, 0xFF, 0 }}
};

void RDevLedSetPattern(LedPatternSeq* ipSeq) {
	ubPatternPtr = 0;
	ubPatternCounter = ipSeq->repeats;
	ptPatternSeq = ipSeq;
	usTickCounter = 0;
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
		PcaLed(pData[2]);
		return RDERR_DONE;
	}
	return RDERR_NOT_SUPPORTED;
}

RDevErrCode RDevLedTick(uint8_t port, uint32_t time)
{
	if (ptPatternSeq) {

		usTickCounter++;
		if  ((usTickCounter>>4) > ptPatternSeq->pt[ubPatternPtr].ticks) {
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
			PcaSetLed(ptPatternSeq->pt[ubPatternPtr].r, ptPatternSeq->pt[ubPatternPtr].g, ptPatternSeq->pt[ubPatternPtr].b);
		}
	}

	return RDERR_DONE;
}
