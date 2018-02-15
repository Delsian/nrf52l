/*
 * r0b1c_device.h
 *
 *  Created on: 15.02.18
 *      Author: ekrashtan
 */

#ifndef DEVICES_R0B1C_DEVICE_H_
#define DEVICES_R0B1C_DEVICE_H_
#include <stdint.h>

// Error codes
typedef enum {
	RDERR_OK,
	RDERR_UNKNOWN_DEVICE,
	RDERR_NOT_SUPPORTED
} RDevErrCode;

typedef enum {
	RDEV_DUMMY = 0x00,
	RDEV_MOTOR,
	RDEV_BUTTON,
	//====
	RDEV_INTERNAL = 0x20, // Not used for real device, for devid checking only
	RDEV_LED,
	RDEV_GYRO,
	RDEV_BUZZ,
	RDEV_BATTERY,
	//====
	RDEV_LAST
} RDevType;

typedef RDevErrCode (*RDevInit)(uint8_t port);
typedef RDevErrCode (*RDevCmd)(uint8_t port, const uint8_t* pData, uint8_t len);

typedef struct _RDevDescriptor {
	RDevType id;
	RDevInit hInit;
	RDevCmd hCmd;
} RDevDescriptor;

RDevErrCode RDeviceChange(uint8_t port, RDevType id);
RDevErrCode RDeviceCmd(uint8_t port, const uint8_t* pData, uint8_t len);
void RDeviceInit(void);

#endif /* DEVICES_R0B1C_DEVICE_H_ */
