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
				.pwm = 2,
				.in1 = 3,
				.in2 = 4,

				.logic1 = 12,
				.logic2 = 13
		},
// Port1
		{
				.pwm = 5,
				.in1 = 6,
				.in2 = 7,

				.logic1 = 14,
				.logic2 = 15
		},
// Port2
		{
				.pwm = 8,
				.in1 = 9,
				.in2 = 10,

				.logic1 = 18,
				.logic2 = 16
		},
// Port3
		{
				.pwm = 11,
				.in1 = 12,
				.in2 = 13,

				.logic1 = 19,
				.logic2 = 17
		}
};

void RjPortResetPwm(uint8_t port)
{
	PcaInit();
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
