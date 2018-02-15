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
#include "nrf_drv_twi.h"
#include "app_util_platform.h"

#define MPU6050_ADDR 0x69
static const nrf_drv_twi_t tMpuDrv = NRF_DRV_TWI_INSTANCE(1);

static nrf_drv_twi_config_t tMpuConfig = {
	.frequency          = NRF_TWI_FREQ_100K,
	.interrupt_priority = APP_IRQ_PRIORITY_LOWEST,
	.clear_bus_init     = false,
	.hold_bus_uninit	= true
};

uint16_t GyroGetVal(uint16_t valId)
{
	return 0;
}

void GyroInit(uint8_t port)
{
	ret_code_t err_code;

	// configure
	tMpuConfig.scl = ExtPorts[port].logic1;
	tMpuConfig.sda = ExtPorts[port].logic2;
	err_code = nrf_drv_twi_init(&tMpuDrv, &tMpuConfig, NULL, NULL);
	APP_ERROR_CHECK(err_code);
	nrf_drv_twi_enable(&tMpuDrv);
	uint8_t ubData[16];
//	ubData[0] = 0x68;
//	ubData[1] = 7;
//	nrf_drv_twi_tx(&tMpuDrv, MPU6050_ADDR, ubData, 2, false);
	ubData[0] = 0x38;
	ubData[1] = 14;
	nrf_drv_twi_tx(&tMpuDrv, MPU6050_ADDR, ubData, 2, true);
	nrf_drv_twi_rx(&tMpuDrv, MPU6050_ADDR, ubData, 14);
	nrf_drv_twi_disable(&tMpuDrv);
}
