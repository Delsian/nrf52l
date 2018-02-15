/*
 * dummy.c
 * Empty device on startup
 *
 *  Created on: 22 ñ³÷. 2018 ð.
 *      Author: ekrashtan
 */

#include <stdint.h>
#include "rj_port.h"
#include "r0b1c_device.h"

RDevErrCode RDevDummyInit(uint8_t port)
{
	// GPIO as input

	// PWM off

	return RDERR_OK;
}

RDevErrCode RDevDummyCmd(uint8_t port, const uint8_t* pData, uint8_t len)
{
	return RDERR_NOT_SUPPORTED;
}
