/*
 * range.c
 *
 *  Created on: 12-03-18
 *      Author: ekrashtan
 */

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "rj_port.h"
#include "r0b1c_device.h"
#include "r0b1c_cmd.h"
#define SENSING_PERIOD_MASK (0x07)

static uint8_t tNotificationMask;
static uint16_t usRange[4];

RDevErrCode RDevRangeInit(uint8_t port)
{
	// Set pin4 as input/pullup
	RjPortSetPin1asInput(port);
	RjPortSetPin2asOutput(port);
	return RDERR_OK;
}

RDevErrCode RDevRangeTick(uint8_t port, uint32_t time)
{
	if ((time & SENSING_PERIOD_MASK) == SENSING_PERIOD_MASK) {

	}
	return RDERR_DONE;
}

RDevErrCode RDevRangeCmd(const uint8_t* pData, uint8_t len)
{
	uint8_t port = pData[0];
	RDevCmdCode ubCommand = (RDevCmdCode)pData[1];
	if(ubCommand == RDCMD_GET) {
		uint8_t pubResp[4] = {pData[0], pData[1], (uint8_t)(usRange[port] & 0xFF), (uint8_t)(usRange[port]>>8)};
		SendCmdResp(pubResp, 4);
		return RDERR_OK;
	} else if (ubCommand == RDCMD_SET && len > 2) {
		if (pData[2] > 0) {
			tNotificationMask ^= (1<<port);
		} else {
			tNotificationMask &= ~(1<<port);
		}
		return RDERR_DONE;
	}
	return RDERR_NOT_SUPPORTED;
}
