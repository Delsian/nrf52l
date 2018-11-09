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
#include "nrfx_clock.h"
#include "nrfx_pwm.h"

static nrfx_pwm_t tBuzzerPwm = NRFX_PWM_INSTANCE(0);
static uint8_t usLoud = 2;

static nrf_pwm_values_common_t seq_values[1];
nrf_pwm_sequence_t const seq =
{
    .values.p_common = seq_values,
    .length          = NRF_PWM_VALUES_LENGTH(seq_values),
    .repeats         = 0,
    .end_delay       = 0
};

// base octave, frequency will double every 12 notes
const nrf_pwm_values_common_t notes[12] = {
		440, 466, 494, 523,
		554, 587, 622, 659,
		699, 740, 784, 831
};

// 700 = 710 , 2000 = 249, 500 = 995, 100 = 4960, 1000 = 497
void BuzzerPlayTone(uint16_t tone)
{
	if (tone) {
		seq_values[0] = tone-(tone/usLoud+1);
		nrf_pwm_configure(tBuzzerPwm.p_registers,
				NRF_PWM_CLK_1MHz, NRF_PWM_MODE_UP, tone);
		nrfx_pwm_simple_playback(&tBuzzerPwm, &seq, 3, NRFX_PWM_FLAG_LOOP);
	} else {
		seq_values[0] = 0;
		nrfx_pwm_stop(&tBuzzerPwm, false);
	}
}

void BuzzerLoudness(uint8_t loud)
{
	usLoud = loud;
}

void BuzzerInit(void)
{
	nrfx_pwm_config_t config =
	    {
			.output_pins =
				{
					BUZZER_PWM, // channel 0
					NRFX_PWM_PIN_NOT_USED,             // channel 1
					NRFX_PWM_PIN_NOT_USED,             // channel 2
					NRFX_PWM_PIN_NOT_USED,             // channel 3
				},
	        // These are the common configuration options we use for all PWM
	        // instances.
	        .irq_priority = APP_IRQ_PRIORITY_LOW,
			.base_clock   = NRF_PWM_CLK_1MHz,
			.top_value	  = 1000,
	        .count_mode   = NRF_PWM_MODE_UP,
			.load_mode    = NRF_PWM_LOAD_COMMON,
	        .step_mode    = NRF_PWM_STEP_AUTO,
	    };

	APP_ERROR_CHECK(nrfx_pwm_init(&tBuzzerPwm, &config, NULL));
}
