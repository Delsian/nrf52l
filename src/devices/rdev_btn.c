/*
 * rdev_btn.c
 *
 *  Created on: 16.02.18
 *      Author: ekrashtan
 */

#include <stdint.h>
#include "rj_port.h"
#include "r0b1c_device.h"
#include "r0b1c_cmd.h"

RDevErrCode RDevButtonInit(uint8_t port)
{
	// Set pin4 as input/pullup
	RjPortSetPin1asInput(port);
	return RDERR_DONE;
}

RDevErrCode RDevButtonTick(uint8_t port, uint32_t time)
{

	return RDERR_OK;
}

RDevErrCode RDevButtonCmd(const uint8_t* pData, uint8_t len)
{
	RDevCmdCode ubCommand = (RDevCmdCode)pData[1];
	if(ubCommand == RDCMD_GET) {
		uint8_t pubResp[3] = {pData[0], pData[1], 1}; // ToDo: !!!!!!!!!!!!
		SendCmdResp(pubResp, 3);
		return RDERR_OK;
	}
	return RDERR_NOT_SUPPORTED;
}



