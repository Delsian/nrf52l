/*
 * r0b1c_device.h
 *
 *  Created on: 15.02.18
 *      Author: ekrashtan
 */

#ifndef DEVICES_R0B1C_DEVICE_H_
#define DEVICES_R0B1C_DEVICE_H_
#include <stdint.h>

// Command codes
typedef enum {
	RDCMD_RESET,
	RDCMD_ID,
	RDCMD_SET,
	RDCMD_GET
} RDevCmdCode;

// Error codes
typedef enum {
	RDERR_OK,
	RDERR_UNKNOWN_DEVICE,
	RDERR_NOT_SUPPORTED,
	RDERR_INCOMPLETE
} RDevErrCode;

typedef enum {
	RDEV_DUMMY = 0x00,
	RDEV_MOTOR,
	RDEV_SERVO = 0x10,
	RDEV_BUTTON = 0x20,
	RDEV_ULTRASONIC = 0x30,
	RDEV_LIGHT = 0x40,
	//====
	RDEV_INTERNAL = 0xC0, // Not used for real device, for devid checking only
	RDEV_LED = 0xC1,
	RDEV_GYRO,
	RDEV_BUZZ,
	RDEV_BATTERY,
	//====
	RDEV_LAST
} RDevType;

typedef RDevErrCode (*RDevInit)(uint8_t port);
typedef RDevErrCode (*RDevCmd)(const uint8_t* pData, uint8_t len);
typedef RDevErrCode (*RDevTick)(uint8_t port, uint32_t time);

typedef struct _RDevDescriptor {
	RDevType id;
	RDevInit hInit;
	RDevCmd hCmd;
	RDevTick hTick;
} RDevDescriptor;

RDevErrCode RDeviceChange(uint8_t port, RDevType id);
RDevErrCode RDeviceCmd(const uint8_t* pData, uint8_t len);
void RDeviceInit(void);

#endif /* DEVICES_R0B1C_DEVICE_H_ */
