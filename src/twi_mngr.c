/*
 * twi_mngr.c
 * Share TWI events between PCA and Gyro
 *
 *  Created on: 2-Mar-2018
 *      Author: ekrashtan
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "pca9685.h"
#include "boards.h"
#include "nrf.h"
#include "app_error.h"
#include "sdk_errors.h"
#include "nrf_drv_twi.h"
#include "app_util_platform.h"

const nrf_drv_twi_config_t tTwiConfig = {
	.scl                = TWI0_SCL,
	.sda                = TWI0_SDA,
	.frequency          = NRF_TWI_FREQ_400K,
	.interrupt_priority = APP_IRQ_PRIORITY_LOWEST,
	.clear_bus_init     = false
};

static bool isInitialized;
static const nrf_drv_twi_t tTwiDrv = NRF_DRV_TWI_INSTANCE(0);

void TwiMngrInit()
{
	if (!isInitialized) {
		APP_ERROR_CHECK(nrf_drv_twi_init(&tTwiDrv, &tTwiConfig, NULL, NULL));
		nrf_drv_twi_enable(&tTwiDrv);
		isInitialized = true;
	}
}

nrf_drv_twi_t const * TwiGetDrv() {
	return &tTwiDrv;
}
