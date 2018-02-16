/*
 * motor.c
 *
 *  Created on: 22.01.18
 *      Author: ekrashtan
 */

#include <stdbool.h>
#include <stdint.h>
#include "rj_port.h"
#include "pca9685.h"
#include "r0b1c_device.h"

RDevErrCode RDevMotorInit(uint8_t port)
{
	return RDERR_OK;
}

RDevErrCode RDevMotorCmd(const uint8_t* pData, uint8_t len)
{

	return RDERR_OK;
}
