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
#include "rj_port.h"
#include "twi_mngr.h"
#include "r0b1c_device.h"

#define MPU6050_ADDR 0x68

uint16_t GyroGetVal(uint16_t valId)
{
	return 0;
}

RDevErrCode RDevGyroInit(uint8_t port)
{
	ret_code_t err_code;
	TwiMngrInit();
	uint8_t ubData[16];
	ubData[0] = 0x68;
	ubData[1] = 7;
	nrf_drv_twi_tx(TwiGetDrv(), MPU6050_ADDR, ubData, 2, false);
	ubData[0] = 0x38;
	ubData[1] = 14;
	nrf_drv_twi_tx(TwiGetDrv(), MPU6050_ADDR, ubData, 2, true);
	nrf_drv_twi_rx(TwiGetDrv(), MPU6050_ADDR, ubData, 14);
	return RDERR_OK;
}

RDevErrCode RDevGyroCmd(uint8_t port, const uint8_t* pData, uint8_t len)
{

	return RDERR_OK;
}

