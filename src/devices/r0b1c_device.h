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
	RDCMD_CONFIG = 0x00,
	RDCMD_ID = 0x01,
	RDCMD_SET = 0x02,
	RDCMD_GET = 0x03,
	RDCMD_SET2 = 0x04,
	RDCMD_GET2 = 0x05
} RDevCmdCode;

// Error codes
typedef enum {
	RDERR_DONE = 0x00, // If command have no response
	RDERR_OK = 0x01, // Command executed successfully with result in write register
	RDERR_NOT_SUPPORTED = 0x05,
	RDERR_BUSY = 0x0B, // Device is busy
	RDERR_UNKNOWN_DEVICE = 0x0D,
	RDERR_INCOMPLETE = 0x1C,
	RDERR_NOTIMPLEMENTED = 0xDE
} RDevErrCode;

typedef enum {
	RDEV_DUMMY = 0x00,
	RDEV_MOTOR_S = 0x01,
	RDEV_MOTOR_M = 0x02,
	RDEV_MOTOR_L = 0x03,
	RDEV_SERVO = 0x10,
	RDEV_BUTTON = 0x20,
	RDEV_ENCODER = 0x28,
	RDEV_ULTRASONIC = 0x30,
	RDEV_LIGHT = 0x40,
	//====
	RDEV_INTERNAL = 0xC0, // Not used for real device, for devid checking only
	RDEV_LED = 0xC1,
	RDEV_GYRO = 0xC2,
	RDEV_BUZZ = 0xC3,
	RDEV_BATTERY = 0xC4,
	//====
	RDEV_LAST = 0xFD
} RDevType;

typedef RDevErrCode (*RDevInit)(uint8_t port);
typedef RDevErrCode (*RDevCmd)(const uint8_t* pData, uint8_t len);
typedef RDevErrCode (*RDevTick)(uint8_t port, uint32_t time);

typedef struct _RDevDescriptor {
	RDevType id;
	RDevInit hInit;
	RDevInit hUnInit; // Call on device removal (stop timers, free memory etc.)
	RDevCmd hCmd;
	RDevTick hTick;
} RDevDescriptor;

RDevErrCode RDeviceChange(uint8_t port, RDevType id);
RDevErrCode RDeviceCmd(const uint8_t* pData, uint8_t len);
RDevType GetDevId(uint8_t port);
void RDeviceInit(void);

#endif /* DEVICES_R0B1C_DEVICE_H_ */
