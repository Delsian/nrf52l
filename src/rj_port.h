/*
 * rj_port.h
 *
 *  Created on: 22 ñ³÷. 2018 ð.
 *      Author: ekrashtan
 */

#ifndef RJ_PORT_H_
#define RJ_PORT_H_

#define TOTAL_RJ_PORTS 4

typedef enum {
	DEV_DUMMY,
	DEV_MOTOR,
	DEV_BUTTON,
	DEV_RANGE
} RjPortDevice;

typedef struct {
	uint8_t num;  // logical port number

	uint8_t pwm; // pin number in PCA9685
	uint8_t in1;
	uint8_t in2;

	uint8_t logic1; // CPU pin to CON1
	uint8_t logic2; // CPU pin to CON2
} RjPortNumbering;

typedef void (*RjPortWritePowerF)(int16_t power);
typedef uint16_t (*RjPortReadValueF)(uint16_t valId);

typedef struct {
	RjPortDevice DevId;
	RjPortWritePowerF WritePower;
	RjPortReadValueF GetVal;
} RjPortSetting;

extern const RjPortNumbering AllPorts[];

void RjPortInit(void);
void RjPortConnect(uint8_t port, RjPortDevice dev );

#endif /* RJ_PORT_H_ */
