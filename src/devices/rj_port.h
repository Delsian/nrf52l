/*
 * rj_port.h
 *
 *  Created on: 22.01.18
 *      Author: ekrashtan
 */

#ifndef RJ_PORT_H_
#define RJ_PORT_H_
#include <stdbool.h>

#define TOTAL_RJ_PORTS 4

typedef struct {
	uint8_t pwm; // pin number in PCA9685
	uint8_t in1;
	uint8_t in2;
	bool	pwmdir;  // indicates inverted PWM connection

	uint8_t logic1; // CPU pin to CON1, blue
	uint8_t logic2; // CPU pin to CON2, yellow
} RjPortPins;

/**
 * Encode pin number by wire color in cable
 */
typedef enum {
	PinBlue = 1,
	PinYellow = 2
} PinColor;

/**
 * Set PWM to disconnected state
 */
void RjPortResetPwm(uint8_t port);

/**
 * Set logical pins to input/nopull state
 */
void RjPortResetInputs(uint8_t port);

/**
 * Write value to PWM
 */
void RjPortSetPwmOut(uint8_t port, int16_t val);

/**
 * Get pin number by port and wire color
 */
uint32_t RjPortGetPin(uint8_t port, PinColor c);

/**
 * Set pin mode to input/pullup
 */
void RjPortSetInput(uint8_t port, PinColor c);

/**
 *
 */
void RjPortSetOutput(uint8_t port, PinColor c);

/**
 * Set pin mode to output
 */
void RjPortPinSet(uint8_t port, PinColor c, uint8_t value);

/**
 * Get pin state
 */
uint32_t RjPortGetVal(uint8_t port, PinColor c);

#endif /* RJ_PORT_H_ */
