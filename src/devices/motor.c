/*
 * motor.c
 *
 *  Created on: 22 ��. 2018 �.
 *      Author: ekrashtan
 */

#include <stdbool.h>
#include <stdint.h>
#include "rj_port.h"
#include "pca9685.h"

static void MotorWritePower(int16_t power)
{

}

void MotorInit(RjPortSetting* settings, uint8_t port)
{
	settings->WritePower = &MotorWritePower;
	settings->DevId = DEV_MOTOR;
	//settings->GetVal = NULL;

	// configure

}
