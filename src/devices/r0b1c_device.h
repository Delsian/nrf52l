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
	RDEV_DUMMY,
	RDEV_MOTOR,
	RDEV_BUTTON,
	//====
	RDEV_LED,
	RDEV_BATTERY
} RDevType;

typedef void (RDevInit)(uint8_t port);

typedef struct _RDevDescriptor {
	RDevType id;
} RDevDescriptor;

RDevErrCode RDeviceChange(uint8_t port, RDevType id);

#endif /* DEVICES_R0B1C_DEVICE_H_ */
