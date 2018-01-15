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
#include "nrf_drv_clock.h"
#include "low_power_pwm.h"

#define BUZZER_PWM_MASK (1<<BUZZER_PWM)

static low_power_pwm_t tBuzzerPwm;
static uint8_t usLoud = 5;

static void pwm_handler(void * p_context)
{
    low_power_pwm_t * pwm_instance = (low_power_pwm_t*)p_context;

    if (pwm_instance->bit_mask == BUZZER_PWM_MASK)
    {

    }
    else
    {
        /*empty else*/
    }
}

void BuzzerPlayTone(uint8_t tone)
{
	if (tone) {
		tBuzzerPwm.period = tone;
		uint16_t l = tone<<4;
		l /= usLoud;
		//tBuzzerPwm.duty_cycle = l&0xFF;
		low_power_pwm_start((&tBuzzerPwm), BUZZER_PWM_MASK);
	} else {
		low_power_pwm_stop(&tBuzzerPwm);
	}
}

void BuzzerLoudness(uint8_t loud)
{
	usLoud = loud;
}

void BuzzerInit(void)
{
	uint32_t err_code;
	low_power_pwm_config_t low_power_pwm_config;

	APP_TIMER_DEF(lpp_timer_0);
	low_power_pwm_config.active_high    = true;
	low_power_pwm_config.period         = 30;
	low_power_pwm_config.bit_mask       = BUZZER_PWM_MASK;
	low_power_pwm_config.p_timer_id     = &lpp_timer_0;
	low_power_pwm_config.p_port			= NRF_GPIO;

	err_code = low_power_pwm_init((&tBuzzerPwm), &low_power_pwm_config, pwm_handler);
	APP_ERROR_CHECK(err_code);
	err_code = low_power_pwm_duty_set(&tBuzzerPwm, 20);
	APP_ERROR_CHECK(err_code);

	//BuzzerPlayTone(0);
}
