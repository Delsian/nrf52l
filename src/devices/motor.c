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

typedef enum {
	None,
	MotorS,
	MotorM,
	MotorL
} MotorSize;

static uint16_t usPortTimer[4];
static MotorSize usMotorSize[4];

RDevErrCode RDevMotorLInit(uint8_t port)
{
	usMotorSize[port] = MotorL;
	return RDERR_OK;
}

RDevErrCode RDevMotorMInit(uint8_t port)
{
	usMotorSize[port] = MotorM;
	return RDERR_OK;
}

RDevErrCode RDevMotorSInit(uint8_t port)
{
	usMotorSize[port] = MotorS;
	return RDERR_OK;
}

RDevErrCode RDevMotorDeInit(uint8_t port)
{
	usMotorSize[port] = None;
	RjPortResetPwm(port);
	return RDERR_OK;
}

// speed = [-127..127]
void RDevMotorWrite(uint8_t port, int8_t speed, uint8_t time) {

	// ToDo: convert speed to PWM value depending of motor size
	uint16_t pv = speed*8;

	if (speed==0) {
		RjPortResetPwm(port);
	} else {
		RjPortSetPwmOut( port, pv);
		if (time)
			usPortTimer[port] = time*16;
	}
}

RDevErrCode RDevMotorCmd(const uint8_t* pData, uint8_t len)
{
	uint8_t port = pData[0];
	RDevCmdCode ubCommand = (RDevCmdCode)pData[1];
	switch (ubCommand) {
		case RDCMD_SET:
			if (len>3) {
				// 1st byte (pData[2]) - power, 2nd (pData[3]) - ticks x16
				RDevMotorWrite(port, (int8_t) pData[2], pData[3]);
				return RDERR_DONE;
			} else {
				return RDERR_INCOMPLETE;
			}
		default:
			break;
	}
	return RDERR_NOT_SUPPORTED;
}

RDevErrCode RDevMotorTick(uint8_t port, uint32_t time)
{
	if (usPortTimer[port] > 0) {
		if (--usPortTimer[port] == 0) {
			RjPortResetPwm(port);
		}
	}
	return RDERR_OK;
}
