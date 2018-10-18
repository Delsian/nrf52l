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
#include "nrfx_twi.h"
#include "nrf_twi_mngr.h"
#include "app_util_platform.h"

const nrfx_twi_config_t tTwiConfig = {
	.scl                = TWI0_SCL,
	.sda                = TWI0_SDA,
	.frequency          = NRF_TWI_FREQ_400K,
	.interrupt_priority = APP_IRQ_PRIORITY_LOWEST,
	.hold_bus_uninit     = true
};

static bool isInitialized;
static const nrfx_twi_t tTwiDrv = NRFX_TWI_INSTANCE(MASTER_TWI_INST);

void TwiMngrInit()
{
	if (!isInitialized) {
		APP_ERROR_CHECK(nrfx_twi_init(&tTwiDrv, &tTwiConfig, NULL, NULL));
		nrfx_twi_enable(&tTwiDrv);
		isInitialized = true;
	}
}

nrfx_twi_t const * TwiGetDrv() {
	return &tTwiDrv;
}
