/*
 * rj_port.c
 *
 *  Created on: 22 ñ³÷. 2018 ð.
 *      Author: ekrashtan
 */


#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "rj_port.h"

// Init functions for devices
extern void MotorInit(RjPortSetting* settings, uint8_t port);

const RjPortNumbering AllPorts[TOTAL_RJ_PORTS] = {
// Port0
		{
				.num = 0,

				.pwm = 2,
				.in1 = 3,
				.in2 = 4,

				.logic1 = 18,
				.logic2 = 17
		},
// Port1
		{
				.num = 1,

				.pwm = 2,
				.in1 = 3,
				.in2 = 4,

				.logic1 = 18,
				.logic2 = 17
		},
// Port2
		{
				.num = 2,

				.pwm = 2,
				.in1 = 3,
				.in2 = 4,

				.logic1 = 18,
				.logic2 = 17
		},
// Port3
		{
				.num = 3,

				.pwm = 2,
				.in1 = 3,
				.in2 = 4,

				.logic1 = 18,
				.logic2 = 17
		}
};

static RjPortSetting tRjPortSettings[TOTAL_RJ_PORTS];

void RjPortConnect(uint8_t port, RjPortDevice dev )
{
	switch(dev)
	{
	case DEV_MOTOR:
		MotorInit(&(tRjPortSettings[dev]), dev);
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

