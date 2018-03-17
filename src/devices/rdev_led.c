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

static LedPatternSeq *ptPatternSeq;
static uint8_t ubPatternPtr;
static uint16_t usTickCounter;
static LedColor tExternalColor;
static uint16_t usIndication;

const LedPatternSeq tPatterns[] = {
		{
				.type = LED_IND_IDLE,
				.length = 3,
				.pt = {
						{20, COLOR_GREEN},
						{10, COLOR_TEAL},
						{100, COLOR_BLACK}
				}
		},

		{
				.type = LED_IND_BTCONN,
				.length = 2,
				.pt = {
						{5, COLOR_BLUE},
						{40, COLOR_BLACK}
				}
		},

		{
				.type = LED_IND_CHARGED,
				.length = 2,
				.pt = {
						{210, COLOR_GREEN},
						{30, COLOR_LIME}
				}
		},

		{
				.type = LED_IND_CHARGING,
				.length = 4,
				.pt = {
						{10, COLOR_OLIVE},
						{5, COLOR_SILVER},
						{10, COLOR_YELLOW},
						{15, COLOR_BLACK}
				}
		},

		{
				.type = LED_IND_LOWBATT,
				.length = 2,
				.pt = {
						{5, COLOR_RED},
						{150, COLOR_BLACK}
				}
		},

		{
				.type = LED_IND_LOWBATT|LED_IND_BTCONN,
				.length = 4,
				.pt = {
						{5, COLOR_RED},
						{30, COLOR_BLACK},
						{5, COLOR_BLUE},
						{100, COLOR_BLACK}
				}
		},

		{
				.type = LED_IND_RUNNING,
				.length = 2,
				.pt = {
						{10, COLOR_LIME},
						{5, COLOR_BLACK}
				}
		},

		{
				.type = LED_IND_NONE
		}
};

void RDevLedSetColor(LedColor c) { tExternalColor = c; }

static void RDevLedUpdatePattern() {
	ubPatternPtr = 0;
	if (usIndication & LED_IND_EXTCOLOR) {
		PcaLedColor(tExternalColor);
		ptPatternSeq = 0;
	} else {
		LedPatternSeq* ps = &(tPatterns[0]);
		while (ps->type != LED_IND_NONE) {
			if ((usIndication & ps->type) == ps->type) {
				ptPatternSeq = ps;
			}
			ps++;
		}
	}
	usTickCounter = 0x8000; // bigger than max counter value to activate LED init on next tick
}

void RDevLedSetIndication(LedIndication ind)
{
	usIndication ^= ind;
	RDevLedUpdatePattern();
}

void RDevLedClearIndication(LedIndication ind)
{
	usIndication &= ~ind;
	RDevLedUpdatePattern();
}

RDevErrCode RDevLedInit(uint8_t port)
{
	tExternalColor = COLOR_BLACK;
	if (usIndication == LED_IND_NONE) {
		// if not initialized by other modules
		usIndication = LED_IND_IDLE;
		RDevLedUpdatePattern();
	}
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
			RDevLedSetIndication(LED_IND_EXTCOLOR);
			RDevLedUpdatePattern();
		} else {
			RDevLedClearIndication(LED_IND_EXTCOLOR);
			RDevLedUpdatePattern();
		}
		return RDERR_DONE;
	default:
		break;
	}
	return RDERR_NOT_SUPPORTED;
}

RDevErrCode RDevLedTick(uint8_t port, uint32_t time)
{
	int i;

	if (ptPatternSeq) {
		usTickCounter++;
		if  ((usTickCounter>>4) > (ptPatternSeq->pt[ubPatternPtr].ticks)) {
			if(++ubPatternPtr >=  ptPatternSeq->length) {
				ubPatternPtr = 0;
			}
			usTickCounter = 0;
			PcaLedColor(ptPatternSeq->pt[ubPatternPtr].color);
		}
	}

	return RDERR_DONE;
}
