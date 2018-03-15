/*
 * rj_port.c
 *
 *  Created on: 22.01.18
 *      Author: ekrashtan
 */


#include <stdint.h>
#include <stdlib.h>
#include "boards.h"
#include "nrf_gpio.h"
#include "rj_port.h"
#include "pca9685.h"

static const RjPortPins ExtPorts[TOTAL_RJ_PORTS] = {
// Port0 J4
		{
				.pwm = PCA9685_PWMB,
				.in1 = PCA9685_IN1A,
				.in2 = PCA9685_IN2A,
				.pwmdir = true,

				.logic1 = PIN_EXT7,
				.logic2 = PIN_EXT1
		},
// Port1 J5
		{
				.pwm = PCA9685_PWMA,
				.in1 = PCA9685_IN1B,
				.in2 = PCA9685_IN2B,
				.pwmdir = false,

				.logic1 = PIN_EXT2,
				.logic2 = PIN_EXT3
		},
// Port2 J6
		{
				.pwm = PCA9685_PWMD,
				.in1 = PCA9685_IN1C,
				.in2 = PCA9685_IN2C,
				.pwmdir = true,

				.logic1 = PIN_EXT0,
				.logic2 = PIN_EXT6
		},
// Port3 J7
		{
				.pwm = PCA9685_PWMC,
				.in1 = PCA9685_IN1D,
				.in2 = PCA9685_IN2D,
				.pwmdir = false,

				.logic1 = PIN_EXT5,
				.logic2 = PIN_EXT4
		}
};

#define PIN_SELECT(color) ((color==PinBlue)?ExtPorts[port].logic1:ExtPorts[port].logic2)

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


uint32_t RjPortGetPin(uint8_t port, PinColor c)
{
	return PIN_SELECT(c);
}

void RjPortSetInput(uint8_t port, PinColor c)
{
	nrf_gpio_cfg_input(PIN_SELECT(c), NRF_GPIO_PIN_PULLUP);
}

void RjPortSetOutput(uint8_t port, PinColor c)
{
	nrf_gpio_cfg_output(PIN_SELECT(c));
}


void RjPortPinSet(uint8_t port, PinColor c, uint8_t value)
{
	if (value) {
		nrf_gpio_pin_set(PIN_SELECT(c));
	} else {
		nrf_gpio_pin_clear(PIN_SELECT(c));
	}
}

uint32_t RjPortGetVal(uint8_t port, PinColor c) {
	return nrf_gpio_pin_read(PIN_SELECT(c));
}


