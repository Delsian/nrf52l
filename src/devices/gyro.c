/*
 * gyro.c
 *
 *  Created on: Jan 22, 2018
 *      Author: Eug
 */


#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "boards.h"
#include "nrf.h"
#include "nrf_log.h"
#include "rj_port.h"
#include "twi_mngr.h"
#include "r0b1c_device.h"
#include "r0b1c_cmd.h"

#define MPU6050_ADDR 			0x68

#define MPU6050_PWR_MGMT_1		0x6B   // R/W
#define MPU6050_PWR_MGMT_2		0x6C   // R/W
#define MPU6050_WHO_AM_I		0x75   // R

static uint8_t ubData[16];
static bool eGyroPresent;

uint16_t GyroGetVal(uint16_t valId)
{
	ubData[0] = 0x38;
	ubData[1] = 14;
	nrf_drv_twi_tx(TwiGetDrv(), MPU6050_ADDR, ubData, 2, true);
	nrf_drv_twi_rx(TwiGetDrv(), MPU6050_ADDR, ubData, 14);
	return 0;
}

RDevErrCode RDevGyroInit(uint8_t port)
{
	ret_code_t err_code;
	TwiMngrInit();
	// check version
	const uint8_t req = MPU6050_WHO_AM_I;
	uint8_t ans;
	nrf_drv_twi_tx(TwiGetDrv(), MPU6050_ADDR, &req, 1, true);
	nrf_drv_twi_rx(TwiGetDrv(), MPU6050_ADDR, &ans, 1);
	//NRF_LOG_DEBUG("Gyro init %d", ans);
	if (ans == 0x68)
	{
		eGyroPresent = true;
		// Turn MPU on
		ubData[0] = MPU6050_PWR_MGMT_1;
		ubData[1] = 0;
		nrf_drv_twi_tx(TwiGetDrv(), MPU6050_ADDR, ubData, 2, false);
		GyroGetVal(0);
	}
	return RDERR_DONE;
}

RDevErrCode RDevGyroTick(uint8_t port, uint32_t time)
{
	static uint8_t count;
	if (eGyroPresent && count++ > 200) {
		count = 0;
		NRF_LOG_DEBUG("Gyro %d %d %d", 1, 2, 3);
	}
	return RDERR_OK;
}

RDevErrCode RDevGyroCmd(uint8_t port, const uint8_t* pData, uint8_t len)
{
	if (eGyroPresent) {
		RDevCmdCode ubCommand = (RDevCmdCode)pData[1];
		switch (ubCommand) {
			case RDCMD_GET:
				SendCmdResp(ubData, 14);
				return RDERR_OK;
			default:
				break;
		}
	}
	return RDERR_NOT_SUPPORTED;
}

