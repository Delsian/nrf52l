/*
 * rdev_led.c
 *
 *  Created on: Feb 18, 2018
 *      Author: Eug
 */

#include <stdint.h>
#include "pca9685.h"
#include "boards.h"
#include "r0b1c_device.h"
#include "r0b1c_cmd.h"

RDevErrCode RDevLedInit(uint8_t port)
{
	PcaPinOn(PCA9685_LEDG);
	PcaPinOn(PCA9685_LEDB);
	PcaPinOn(PCA9685_LEDR);
	return RDERR_DONE;
}

RDevErrCode RDevLedCmd(const uint8_t* pData, uint8_t len)
{
	RDevCmdCode ubCommand = (RDevCmdCode)pData[1];
	if(ubCommand == RDCMD_SET) {
		PcaLed(pData[2]);
		return RDERR_DONE;
	}
	return RDERR_NOT_SUPPORTED;
}

RDevErrCode RDevLedTick(uint8_t port, uint32_t time)
{
	static int i;
	if (--i < 1) i=200;
	if (i>100)
		PcaPinOn(PCA9685_LEDB);
	else
		PcaPinOff(PCA9685_LEDB);
	return RDERR_DONE;
}
