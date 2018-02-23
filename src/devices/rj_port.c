/*
 * rj_port.c
 *
 *  Created on: 22.01.18
 *      Author: ekrashtan
 */


#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "boards.h"
#include "nrf_gpio.h"
#include "rj_port.h"
#include "pca9685.h"

static const RjPortPins ExtPorts[TOTAL_RJ_PORTS] = {
// Port0
		{
				.pwm = PCA9685_PWMA,
				.in1 = PCA9685_IN1A,
				.in2 = PCA9685_IN2A,

				.logic1 = PIN_EXT7,
				.logic2 = PIN_EXT1
		},
// Port1
		{
				.pwm = PCA9685_PWMB,
				.in1 = PCA9685_IN1B,
				.in2 = PCA9685_IN2B,

				.logic1 = PIN_EXT2,
				.logic2 = PIN_EXT3
		},
// Port2
		{
				.pwm = PCA9685_PWMC,
				.in1 = PCA9685_IN1C,
				.in2 = PCA9685_IN2C,

				.logic1 = PIN_EXT0,
				.logic2 = PIN_EXT6
		},
// Port3
		{
				.pwm = PCA9685_PWMD,
				.in1 = PCA9685_IN1D,
				.in2 = PCA9685_IN2D,

				.logic1 = PIN_EXT5,
				.logic2 = PIN_EXT4
		}
};

void RjPortResetPwm(uint8_t port)
{
	// Set to high impedance - In1,In2 = 0, PWM = 1
	PcaPinOff(ExtPorts[port].in1);
	PcaPinOff(ExtPorts[port].in2);
	PcaPinOn(ExtPorts[port].pwm);
}

void RjPortResetInputs(uint8_t port)
{
	nrf_gpio_cfg_input(ExtPorts[port].logic1, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_input(ExtPorts[port].logic2, NRF_GPIO_PIN_NOPULL);
}

void RjPortSetPwmOut(uint8_t port, int16_t val)
{
	uint16_t absval = val;
	if (val > 0) {
		PcaPinOff(ExtPorts[port].in1);
		PcaPinOn(ExtPorts[port].in2);
	} else {
		absval = -val;
		PcaPinOn(ExtPorts[port].in1);
		PcaPinOff(ExtPorts[port].in2);
	}
	PcaWriteChannel(ExtPorts[port].pwm, absval);
}

void RjPortSetPin1asInput(uint8_t port)
{
	nrf_gpio_cfg_input(ExtPorts[port].logic1, NRF_GPIO_PIN_PULLUP);
}
