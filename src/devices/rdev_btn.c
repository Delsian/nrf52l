/*
 * rdev_btn.c
 * External button - touch sensor
 *  Created on: 16.02.18
 *      Author: ekrashtan
 */

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "rj_port.h"
#include "r0b1c_device.h"
#include "r0b1c_cmd.h"

static uint8_t ubBtnState[4];
static uint8_t tNotificationMask;

RDevErrCode RDevButtonInit(uint8_t port)
{
	// Set pin4 as input/pullup
	RjPortSetPin1asInput(port);
	ubBtnState[port] = 0;
	return RDERR_DONE;
}

static void RDevButtonNotify(uint8_t port, bool state)
{
	uint8_t notif[3] = {port, RDCMD_GET, state};
	if (tNotificationMask & (1<<port)) {
		SendCmdNotif(notif, 3);
	}
}

RDevErrCode RDevButtonTick(uint8_t port, uint32_t time)
{
	assert(port<4);

	if (RjPortGetPin1(port) == 0) { // if pressed
		if (++ubBtnState[port] == 2) {
			// send "press" event on 2nd tick
			RDevButtonNotify(port, true);
		}
		ubBtnState[port] &= 0x7; // avoid overflow
	} else {
		if (ubBtnState[port] > 2) {
			// send "release"
			RDevButtonNotify(port, false);
		}
		ubBtnState[port] = 0;
	}
	return RDERR_OK;
}

RDevErrCode RDevButtonCmd(const uint8_t* pData, uint8_t len)
{
	uint8_t port = pData[0];
	RDevCmdCode ubCommand = (RDevCmdCode)pData[1];
	if(ubCommand == RDCMD_GET) {
		uint8_t pubResp[3] = {pData[0], pData[1], (ubBtnState[port] > 2)?1:0};
		SendCmdResp(pubResp, 3);
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



