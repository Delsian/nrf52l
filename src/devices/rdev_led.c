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
#include "nrf_log.h"

static LedPatternSeq *ptPatternSeq, *ptPatternKeep;
static uint8_t ubPatternCounter; // Loops. If pattern counter = 0xFF - continuous
static uint8_t ubPatternPtr;
static uint16_t usTickCounter;
static LedColor tExternalColor;

const LedPatternSeq tPatternIdle = {
		.repeats = 0xFF,
		.length = 4,
		.pt = { {0x38, COLOR_CYAN },
				{0x38, COLOR_GREEN },
				{0x38, COLOR_FIREBRICK },
				{0x40, COLOR_MAGENTA } }
};

void RDevLedSetPattern(const LedPatternSeq * ipSeq) {
	ptPatternKeep = ptPatternSeq;
	ubPatternPtr = 0;
	ubPatternCounter = ipSeq->repeats;
	ptPatternSeq = (LedPatternSeq *)ipSeq;
	usTickCounter = 0x8000; // bigger than max counter value to activate LED init on next tick
	NRF_LOG_DEBUG("set %p", ipSeq);
}

void RDevLedStopPattern(const LedPatternSeq* ipSeq) {
	if (ptPatternKeep == ipSeq) ptPatternKeep = NULL;
	else if (ptPatternSeq == ipSeq) {
		RDevLedSetPattern(ptPatternKeep);
	}
}

RDevErrCode RDevLedInit(uint8_t port)
{
	tExternalColor = COLOR_BLACK;
	ptPatternKeep = NULL;
	RDevLedSetPattern(&tPatternIdle);
	return RDERR_DONE;
}

RDevErrCode RDevLedCmd(const uint8_t* pData, uint8_t len)
{
	RDevCmdCode ubCommand = (RDevCmdCode)pData[1];
	switch (ubCommand) {
	case RDCMD_SET:
		if (len>3) {
			uint16_t col = pData[3] * 256 + pData[2];
			tExternalColor = (LedColor)col;
			// Turn off pattern blinking
			ubPatternPtr = 0xFF;
			PcaLedColor(tExternalColor);
			return RDERR_DONE;
		} else {
			return RDERR_INCOMPLETE;
		}
	case RDCMD_RESET:
		RDevLedInit(0);
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
						if (ptPatternKeep) { // restore LED state on temporary pattern end
							RDevLedSetPattern(ptPatternKeep);
							ptPatternKeep = NULL;
						} else {
							PcaLedColor(tExternalColor);
							ptPatternSeq = NULL;
						}
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
