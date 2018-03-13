/*
 * r0b1c_device.c
 *
 *  Created on: 15.02.18
 *      Author: ekrashtan
 */

#include <stdlib.h>
#include "app_timer.h"
#include "nrf_drv_gpiote.h"
#include "nrf_log.h"
#include "r0b1c_device.h"
#include "pca9685.h"
#include "rj_port.h"
#include "custom_service.h"
#include "r0b1c_cmd.h"

#define RDEV_TICK_TIMEOUT 50

// Prototypes
RDevErrCode RDevDummyInit(uint8_t port);
RDevErrCode RDevMotorInit(uint8_t port);
RDevErrCode RDevMotorCmd(const uint8_t* pData, uint8_t len);
RDevErrCode RDevMotorTick(uint8_t port, uint32_t time);
RDevErrCode RDevButtonInit(uint8_t port);
RDevErrCode RDevButtonTick(uint8_t port, uint32_t time);
RDevErrCode RDevButtonCmd(const uint8_t* pData, uint8_t len);
RDevErrCode RDevRangeInit(uint8_t port);
RDevErrCode RDevRangeTick(uint8_t port, uint32_t time);
RDevErrCode RDevRangeCmd(const uint8_t* pData, uint8_t len);
RDevErrCode RDevLedInit(uint8_t port);
RDevErrCode RDevLedCmd(const uint8_t* pData, uint8_t len);
RDevErrCode RDevLedTick(uint8_t port, uint32_t time);
RDevErrCode RDevGyroInit(uint8_t port);
RDevErrCode RDevGyroCmd(const uint8_t* pData, uint8_t len);
RDevErrCode BatteryTick(uint8_t port, uint32_t time);
RDevErrCode RDevBattCmd(const uint8_t* pData, uint8_t len);
RDevErrCode BatteryInit(uint8_t port);

const RDevDescriptor ptRDevices[] = {
		// Dummy - always on first position as zero device. Other records can vary
		{
				.id = RDEV_DUMMY,
				.hInit = &RDevDummyInit
		},
		// Motor
		{
				.id = RDEV_MOTOR,
				.hInit = &RDevMotorInit,
				.hCmd = &RDevMotorCmd,
				.hTick = &RDevMotorTick
		},
		// Button
		{
				.id = RDEV_BUTTON,
				.hInit = &RDevButtonInit,
				.hCmd = &RDevButtonCmd,
				.hTick = &RDevButtonTick
		},
		// Range sensor
		{
				.id = RDEV_ULTRASONIC,
				.hInit = &RDevRangeInit,
				.hCmd = &RDevRangeCmd,
				.hTick = &RDevRangeTick
		},
/* Next section for internal devices only */
		// LED
		{
				.id = RDEV_LED,
				.hInit = &RDevLedInit,
				.hCmd = &RDevLedCmd,
				.hTick = &RDevLedTick
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
		// Battery
		{
				.id = RDEV_BATTERY,
				.hTick = &BatteryTick,
				.hInit = &BatteryInit,
				.hCmd = RDevBattCmd
		},


/* End of list */
		{
				.id = RDEV_LAST
		}
};

static RDevDescriptor gpDevInPort[TOTAL_RJ_PORTS];

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
	if (gpDevInPort[port].hUnInit) {
		// De-init old device
		(gpDevInPort[port].hUnInit)(port);
	}
	memcpy(&(gpDevInPort[port]), &(ptRDevices[devindex]), sizeof(RDevDescriptor));
	if (ptRDevices[devindex].hInit)
	{
		return (ptRDevices[devindex].hInit)(port);
	} else {
		return RDERR_OK;
	}
}

RDevErrCode RDeviceCmd(const uint8_t* pData, uint8_t len)
{
	RDevErrCode tErr = RDERR_NOTIMPLEMENTED;
	uint8_t port = pData[0];
	RDevCmdCode tCmd = (RDevCmdCode)pData[1];
	if (port >= TOTAL_RJ_PORTS) {
		// Internal devices - port number equals to device Id
		int id = FindDevById(port);
		if (id > 0) {
			if (ptRDevices[id].hCmd) {
				tErr = (ptRDevices[id].hCmd)(pData, len);
			}
		} else {
			tErr = RDERR_UNKNOWN_DEVICE;
		}
	} else {
		// External devices
		if (gpDevInPort[port].hCmd) {
			// Executre command for external device
			tErr = (gpDevInPort[port].hCmd)(pData, len);
		}
	}
	return tErr;
}

static void RDevTickHandler()
{
	static uint32_t clock;
	RDevErrCode tErr;

	int i = 0;
	for (;i<TOTAL_RJ_PORTS;i++) {
		if (gpDevInPort[i].hTick)
			tErr = (gpDevInPort[i].hTick)(i, clock);
	}
	i = 1;
	while(1) {
		if (ptRDevices[++i].id <= RDEV_INTERNAL) continue;
		if (ptRDevices[i].id == RDEV_LAST) break;
		if (ptRDevices[i].hTick)
			tErr = (ptRDevices[i].hTick)(0, clock);
	}
	clock ++;
}

// Prepare internal devices on startup
void RDeviceInit(void)
{
	PcaInit();
	int i = 0;
	for (;i<TOTAL_RJ_PORTS;i++) {
		// By default - dummy devices on all ports
		(ptRDevices[0].hInit)(i);
	}
	i = 0;
	while(1) {
		if (ptRDevices[++i].id == RDEV_LAST) break;
		if (ptRDevices[i].id <= RDEV_INTERNAL) continue;
		if (ptRDevices[i].hInit)
			(void)(ptRDevices[i].hInit)(0);
	}
	// Init timer
	APP_TIMER_DEF(tRDevTimer);
	app_timer_create(&tRDevTimer, APP_TIMER_MODE_REPEATED, RDevTickHandler);
	app_timer_start(tRDevTimer, RDEV_TICK_TIMEOUT, NULL);

	// Init GPIOTE
	nrf_drv_gpiote_init();
}
