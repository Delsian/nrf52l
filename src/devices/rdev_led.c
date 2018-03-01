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

#define FADE_DIFFERENSE 10 // send i2c transaction not too often
#define FADE_SPEED 4 // How fast color changes

typedef struct {
	uint16_t curr;
	int16_t step;
	uint16_t end;
} FadeChannel;

static LedPatternSeq* ptPatternSeq;
static uint8_t ubPatternCounter; // Loops
static uint8_t ubPatternPtr;
static uint16_t usTickCounter;

const LedPatternSeq tPatternIdle = {
		.repeats = 0xFF,
		.length = 4,
		.patEnd = NULL,
		.pt = { {0xC8, { 0xCF, 0, 0} },
				{0xD8, { 0, 0xF0, 0} },
				{0xD0, { 0, 0, 0xE0} } }
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
		PcaLed(pData[2]);
		return RDERR_DONE;
	}
	return RDERR_NOT_SUPPORTED;
}

RDevErrCode RDevLedTick(uint8_t port, uint32_t time)
{
	static bool isFade;
	static FadeChannel tFch[3];
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
			PcaSetLed(ptPatternSeq->pt[ubPatternPtr].color[0], ptPatternSeq->pt[ubPatternPtr].color[1], ptPatternSeq->pt[ubPatternPtr].color[2]);
			if (ptPatternSeq->pt[ubPatternPtr].ticks & 0x80) { // fade to next state
				isFade = true;
				uint8_t next = (ubPatternPtr > ptPatternSeq->length-2) ? 0:ubPatternPtr+1;
				for (i=0; i<3; i++) {
					tFch[i].curr = ptPatternSeq->pt[ubPatternPtr].color[i] << 8;
					tFch[i].end = ptPatternSeq->pt[next].color[i] << 8;
					tFch[i].step = ((ptPatternSeq->pt[next].color[i] - ptPatternSeq->pt[ubPatternPtr].color[i]) << 8) /
							((ptPatternSeq->pt[next].ticks&0x7F)<<FADE_SPEED);
				}
			} else {
				isFade = false;
			}
		} else if (isFade) {
			// Fade
			bool isNew = false; // Should we update LED state?
			bool isEnd = true;	// Should we stop fading?
			for (i=0; i<3; i++) {
				uint16_t newval = tFch[i].curr + tFch[i].step;
				if (newval>>FADE_DIFFERENSE != tFch[i].curr>>FADE_DIFFERENSE) isNew = true;
				tFch[i].curr = newval;
				if (abs(tFch[i].curr - tFch[i].end) < 0x300) { // Ignore minor color differences before end
						tFch[i].step = 0;
						tFch[i].curr = tFch[i].end;
						isNew = true; // Last write to this color
				}
				if (tFch[i].step != 0 ) isEnd = false;
			}

			if (isNew)
				PcaSetLed(tFch[0].curr>>8,tFch[1].curr>>8,tFch[2].curr>>8);
			isFade = !isEnd;
		}
	}

	return RDERR_DONE;
}
