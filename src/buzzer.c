/*
 * buzzer.c
 *
 *  Created on: Jan 15, 2018
 *      Author: Eug
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "app_error.h"
#include "sdk_errors.h"
#include "app_timer.h"
#include "app_util_platform.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_pwm.h"

static nrf_drv_pwm_t tBuzzerPwm = NRF_DRV_PWM_INSTANCE(0);
static uint8_t usLoud = 5;


void BuzzerPlayTone(uint16_t tone)
{
	if (tone) {
		nrf_pwm_enable(tBuzzerPwm.p_registers);
		nrf_pwm_configure(tBuzzerPwm.p_registers,
				NRF_PWM_CLK_250kHz, NRF_PWM_MODE_UP, tone);
	} else {
		nrf_drv_pwm_stop(&tBuzzerPwm, false);
	}
}

void BuzzerLoudness(uint8_t loud)
{
	usLoud = loud;
}

void BuzzerInit(void)
{
	uint32_t err_code;
	nrf_drv_pwm_config_t config =
	    {
			.output_pins =
				{
					BUZZER_PWM, // channel 0
					NRF_DRV_PWM_PIN_NOT_USED,             // channel 1
					NRF_DRV_PWM_PIN_NOT_USED,             // channel 2
					NRF_DRV_PWM_PIN_NOT_USED,             // channel 3
				},
	        // These are the common configuration options we use for all PWM
	        // instances.
	        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
			.base_clock   = NRF_PWM_CLK_250kHz,
	        .count_mode   = NRF_PWM_MODE_UP,
	        .step_mode    = NRF_PWM_STEP_AUTO,
	    };

	APP_ERROR_CHECK(nrf_drv_pwm_init(&tBuzzerPwm, &config, NULL));

}
