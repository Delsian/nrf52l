/*
 * r0b1c_device.c
 *
 *  Created on: 15.02.18
 *      Author: ekrashtan
 */

#include "r0b1c_device.h"
#include "rj_port.h"

// Prototypes
RDevErrCode RDevDummyInit(uint8_t port);
RDevErrCode RDevDummyCmd(uint8_t port, const uint8_t* pData, uint8_t len);
RDevErrCode RDevMotorInit(int8_t port);
RDevErrCode RDevMotorCmd(uint8_t port, const uint8_t* pData, uint8_t len);

RDevErrCode RDevGyroInit(int8_t port);
RDevErrCode RDevGyroCmd(uint8_t port, const uint8_t* pData, uint8_t len);

const RDevDescriptor ptRDevices[] = {
		// Dummy
		{
				.id = RDEV_DUMMY,
				.hInit = &RDevDummyInit,
				.hCmd = &RDevDummyCmd
		},
		// Motor
		{
				.id = RDEV_MOTOR,
				.hInit = &RDevMotorInit,
				.hCmd = &RDevMotorCmd
		},
		// Button
		{
				.id = RDEV_BUTTON
		},
/* Next section for internal devices only */
		// LED
		{
				.id = RDEV_LED
		},
		// Gyro
		{
				.id = RDEV_GYRO,
				.hInit = &RDevGyroInit,
				.hCmd = &RDevGyroCmd
		},
		// Buzzer
		{
				.id = RDEV_BUZZ
		},

/* End of list */
		{
				.id = RDEV_LAST
		}
};

static RDevCmd gpCmd[TOTAL_RJ_PORTS];

const int FindDevById(RDevType id)
{
	int i = 0;
	while(1) {
		if (ptRDevices[i].id == RDEV_LAST) break;
		if (ptRDevices[i].id == id) return i;
		i++;
	}
	return -1;
}

RDevErrCode RDeviceChange(uint8_t port, RDevType id)
{
	if (id >= RDEV_INTERNAL)
		return RDERR_NOT_SUPPORTED;
	int devindex = FindDevById(id);
	if (devindex < 0)
		return RDERR_UNKNOWN_DEVICE;
	gpCmd[port] = ptRDevices[devindex].hCmd;
	if (ptRDevices[devindex].hInit)
	{
		return (ptRDevices[devindex].hInit)(port);
	} else
		return RDERR_OK;
}

RDevErrCode RDeviceCmd(uint8_t port, const uint8_t* pData, uint8_t len)
{
	if (port > TOTAL_RJ_PORTS) {
		// Internal devices
		int id = FindDevById(port);
		if (id > 0 && ptRDevices[id].hCmd)
			return (ptRDevices[id].hCmd)(port, pData, len);
	} else {
		// External devices
		if (ptRDevices[port].hCmd)
			return (ptRDevices[port].hCmd)(port, pData, len);
	}
	return RDERR_NOT_SUPPORTED;
}

// Prepare internal devices on startup
void RDeviceInit(void)
{
	int i = 0;
	while(1) {
		if (ptRDevices[i].id == RDEV_LAST) break;
		if (ptRDevices[i].id <= RDEV_INTERNAL) continue;
		if (ptRDevices[i].hInit)
			(void)(ptRDevices[i].hInit)(0);
		i++;
	}
}
