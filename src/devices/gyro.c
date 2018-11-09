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
#include "nrf_twi_mngr.h"
#include "nrf_drv_twi.h"
#include "r0b1c_device.h"
#include "r0b1c_cmd.h"
#include "pca9685.h"

#define MPU6050_ADDR 			0x68

#define MPU6050_PWR_MGMT_1		0x6B   // R/W
#define MPU6050_PWR_MGMT_2		0x6C   // R/W
#define MPU6050_WHO_AM_I		0x75   // R

#ifndef USE_TWI_MNGR
#error TWI_MNGR required!
#endif

#define GYROCTL_LEN 14
static void gyro_wr_cb(ret_code_t result, void * p_user_data);
static uint8_t gyroctl[GYROCTL_LEN];
static nrf_twi_mngr_transfer_t transfers[] = {
		NRF_TWI_MNGR_WRITE(MPU6050_ADDR, gyroctl, 2, 0),
		NRF_TWI_MNGR_READ(MPU6050_ADDR, gyroctl, GYROCTL_LEN, 0),
};
static nrf_twi_mngr_transaction_t NRF_TWI_MNGR_BUFFER_LOC_IND transaction =
{
    .callback            = gyro_wr_cb,
    .p_user_data         = NULL,
    .p_transfers         = transfers,
    .number_of_transfers = 1
};
static bool eGyroPresent;

static void gyro_wr_cb(ret_code_t result, void * p_user_data) {}

uint16_t GyroGetVal(uint16_t valId)
{
	memset(gyroctl, 0, 2);
    gyroctl[0] = 0x38;
    gyroctl[1] = 14;
    transaction.number_of_transfers = 2;
	//nrfx_twi_tx(TwiGetDrv(), MPU6050_ADDR, ubData, 2, true);
	//nrfx_twi_rx(TwiGetDrv(), MPU6050_ADDR, ubData, 14);
	return 0;
}

RDevErrCode RDevGyroInit(uint8_t port)
{
	// check version
	const uint8_t req = MPU6050_WHO_AM_I;
	uint8_t ans;
	//nrfx_twi_tx(TwiGetDrv(), MPU6050_ADDR, &req, 1, true);
	//nrfx_twi_rx(TwiGetDrv(), MPU6050_ADDR, &ans, 1);
	//NRF_LOG_DEBUG("Gyro init %d", ans);
	if (ans == 0x68)
	{
		eGyroPresent = true;
		// Turn MPU on
		//ubData[0] = MPU6050_PWR_MGMT_1;
		//ubData[1] = 0;
		//nrfx_twi_tx(TwiGetDrv(), MPU6050_ADDR, ubData, 2, false);
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
				SendCmdResp(gyroctl, 14);
				return RDERR_OK;
			default:
				break;
		}
	}
	return RDERR_NOT_SUPPORTED;
}

