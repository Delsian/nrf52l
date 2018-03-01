/*
 * pca9685.c
 *
 *  Created on: Jan 13, 2018
 *      Author: Eug
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "pca9685.h"
#include "boards.h"
#include "nrf.h"
#include "app_error.h"
#include "sdk_errors.h"
#include "nrf_delay.h"
#include "nrf_drv_twi.h"
#include "app_util_platform.h"
#include "rdev_led.h"

#define PCA9685_ADDR 0x60
#define PCA9685_MAX_CHANNEL         15

// Register addresses from data sheet
#define PCA9685_MODE1_REG           0x00
#define PCA9685_MODE2_REG           0x01
#define PCA9685_SUBADR1_REG         0x02
#define PCA9685_SUBADR2_REG         0x03
#define PCA9685_SUBADR3_REG         0x04
#define PCA9685_ALLCALL_REG         0x05
#define PCA9685_LED0_REG            0x06 // Start of LEDx regs, 4B per reg, 2B on phase, 2B off phase, little-endian
#define PCA9685_PRESCALE_REG        0xFE
#define PCA9685_ALLLED_REG          0xFA

// Mode1 register pin layout
#define PCA9685_MODE_RESTART        0x80
#define PCA9685_MODE_EXTCLK         0x40
#define PCA9685_MODE_AUTOINC        0x20
#define PCA9685_MODE_SLEEP          0x10
#define PCA9685_MODE_SUBADR1        0x08
#define PCA9685_MODE_SUBADR2        0x04
#define PCA9685_MODE_SUBADR3        0x02
#define PCA9685_MODE_ALLCALL        0x01

#define PCA9685_SW_RESET            0x06 // Sent to address 0x00 to reset all devices on Wire line
#define PCA9685_PWM_FULL            0x01000 // Special value for full on/full off LEDx modes

const nrf_drv_twi_config_t tPcaConfig = {
	.scl                = TWI0_SCL,
	.sda                = TWI0_SDA,
	.frequency          = NRF_TWI_FREQ_400K,
	.interrupt_priority = APP_IRQ_PRIORITY_LOWEST,
	.clear_bus_init     = false
};

static const nrf_drv_twi_t tPcaDrv = NRF_DRV_TWI_INSTANCE(0);
static uint8_t ubData[5];

// Set pin to 1
void PcaPinOn(uint8_t ch)
{
	if (ch<=PCA9685_MAX_CHANNEL) {
		ubData[0] = ((ch<<2) + PCA9685_LED0_REG);
		ubData[1] = 0;
		ubData[2] = 0x10;
		ubData[3] = 0;
		ubData[4] = 0;
		nrf_drv_twi_tx(&tPcaDrv, PCA9685_ADDR, ubData, 5, false);
	}
}

// Reset pin to 0
void PcaPinOff(uint8_t ch)
{
	if (ch<=PCA9685_MAX_CHANNEL) {
		ubData[0] = ((ch<<2) + PCA9685_LED0_REG);
		ubData[1] = 0;
		ubData[2] = 0;
		ubData[3] = 0;
		ubData[4] = 0x10;
		nrf_drv_twi_tx(&tPcaDrv, PCA9685_ADDR, ubData, 5, false);
	}
}

// write byte value using auto-increment
void PcaWriteChannel(uint8_t ch, uint8_t val)
{
	if (ch<=PCA9685_MAX_CHANNEL || ch == PCA9685_ALLLED_REG) {
		if (val == 0) {
			PcaPinOff (ch);
		} else if (val == 0xFF) {
			PcaPinOn (ch);
		} else {
			uint16_t val_on = val << 4;
			ubData[0] = (ch == PCA9685_ALLLED_REG)? ch : ((ch<<2) + PCA9685_LED0_REG);
			ubData[1] = 0xFF;
			ubData[2] = 0xF;
			ubData[3] = val_on&0xFF;
			ubData[4] = val_on>>8;
			nrf_drv_twi_tx(&tPcaDrv, PCA9685_ADDR, ubData, 5, false);
		}
	}
}

static uint8_t PcaLedLookupTable(uint8_t val)
{
	const uint8_t table[64] = {
			0,  2,  3,  4,  4,  5,  6,  6,
			7,  8,  9, 10, 11, 12, 13, 15,
			16, 17, 19, 20, 22, 24, 26, 28,
			30, 32, 34, 37, 39, 42, 45, 48,
			51, 54, 57, 61, 65, 68, 72, 77,
			81, 86, 91, 96,101,107,112,118,
			125,131,138,145,152,160,168,176,
			185,194,203,213,223,234,244,255
	};
	return table[val&0x3F];
}

void PcaLedValue(uint8_t r, uint8_t g, uint8_t b) {
	PcaWriteChannel(PCA9685_LEDG, 0xFF - PcaLedLookupTable(g));
	PcaWriteChannel(PCA9685_LEDB, 0xFF - PcaLedLookupTable(b));
	PcaWriteChannel(PCA9685_LEDR, 0xFF - PcaLedLookupTable(r));
}

void PcaLedColor(LedColor color)
{
	PcaWriteChannel(PCA9685_LEDG, 0xFF - PcaLedLookupTable(color>>5));
	PcaWriteChannel(PCA9685_LEDB, 0xFF - PcaLedLookupTable(color<<1));
	PcaWriteChannel(PCA9685_LEDR, 0xFF - PcaLedLookupTable((color>>10)&0x3E));
}

void PcaInit(void)
{
	APP_ERROR_CHECK(nrf_drv_twi_init(&tPcaDrv, &tPcaConfig, NULL, NULL));
	nrf_drv_twi_enable(&tPcaDrv);

	const uint8_t ubDataSet[5][2] = {
			{PCA9685_MODE1_REG, PCA9685_MODE_SLEEP},
			{PCA9685_PRESCALE_REG, 3}, // max pwm freq
			{PCA9685_MODE1_REG, PCA9685_MODE_RESTART},
			{PCA9685_MODE1_REG, PCA9685_MODE_AUTOINC},
			{PCA9685_MODE2_REG, 0x4}
	};
	for (int i=0; i<5;i++) {
		nrf_drv_twi_tx(&tPcaDrv, PCA9685_ADDR, ubDataSet[i], 2, false);
	}
}
