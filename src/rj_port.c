/*
 * rj_port.c
 *
 *  Created on: 22 ñ³÷. 2018 ð.
 *      Author: ekrashtan
 */


#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "rj_port.h"

// Init functions for devices
extern void MotorInit(RjPortSetting* settings, uint8_t port);
extern void GyroInit(RjPortSetting* settings, uint8_t port);

const RjPortNumbering AllPorts[TOTAL_RJ_PORTS] = {
// Port0
		{
				.num = 0,

				.pwm = 2,
				.in1 = 3,
				.in2 = 4,

				.logic1 = 12,
				.logic2 = 13
		},
// Port1
		{
				.num = 1,

				.pwm = 5,
				.in1 = 6,
				.in2 = 7,

				.logic1 = 14,
				.logic2 = 15
		},
// Port2
		{
				.num = 2,

				.pwm = 8,
				.in1 = 9,
				.in2 = 10,

				.logic1 = 18,
				.logic2 = 16
		},
// Port3
		{
				.num = 3,

				.pwm = 11,
				.in1 = 12,
				.in2 = 13,

				.logic1 = 19,
				.logic2 = 17
		}
};

static RjPortSetting tRjPortSettings[TOTAL_RJ_PORTS];

void RjPortConnect(uint8_t port, RjPortDevice dev )
{
	if (tRjPortSettings[dev].Context) {
		// If we have allocated context for previous device
		if (tRjPortSettings[dev].Disconnect) {
			(*tRjPortSettings[dev].Disconnect)();
		} else {
			free(tRjPortSettings[dev].Context);
			tRjPortSettings[dev].Context = 0;
		}
	}

	switch(dev)
	{
	case DEV_MOTOR:
		MotorInit(&(tRjPortSettings[port]), port);
		break;
	case DEV_GYRO:
		GyroInit(&(tRjPortSettings[port]), port);
		break;
	default:
		break;
	}
}

void RjPortInit(void)
{
	// Dummy device with no settings
	memset(tRjPortSettings, 0, sizeof(tRjPortSettings));

}

