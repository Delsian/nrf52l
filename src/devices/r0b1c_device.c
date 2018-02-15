/*
 * r0b1c_device.c
 *
 *  Created on: 15.02.18
 *      Author: ekrashtan
 */

#include "r0b1c_device.h"

static RDevDescriptor ptRDevices[] = {
		// Dummy
		{
				.id = RDEV_DUMMY
		},
		// Motor
		{

		}
};

RDevErrCode RDeviceChange(uint8_t port, RDevType id)
{
	return RDERR_OK;
}
