/*
 * rj_port.h
 *
 *  Created on: 22.01.18
 *      Author: ekrashtan
 */

#ifndef RJ_PORT_H_
#define RJ_PORT_H_

#define TOTAL_RJ_PORTS 4

typedef struct {
	uint8_t pwm; // pin number in PCA9685
	uint8_t in1;
	uint8_t in2;

	uint8_t logic1; // CPU pin to CON1
	uint8_t logic2; // CPU pin to CON2
} RjPortPins;

void RjPortResetPwm(uint8_t port);
void RjPortResetInputs(uint8_t port);
void RjPortSetPwmOut(uint8_t port, int16_t val);
void RjPortSetPin1asInput(uint8_t port);

#endif /* RJ_PORT_H_ */
