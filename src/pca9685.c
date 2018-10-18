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
#include "nrf_twi_mngr.h"
#include "sdk_errors.h"
#include "rdev_led.h"

#define TWI_INSTANCE_ID             0
#define MAX_PENDING_TRANSACTIONS    5

NRF_TWI_MNGR_DEF(m_nrf_twi_mngr, MAX_PENDING_TRANSACTIONS, TWI_INSTANCE_ID);
static bool IsTwiInitialized;

void TwiMngrInit() {
	nrf_drv_twi_config_t const config = {
		.scl                = TWI0_SCL,
		.sda                = TWI0_SDA,
		.frequency          = NRF_TWI_FREQ_400K,
		.interrupt_priority = APP_IRQ_PRIORITY_LOWEST,
		.hold_bus_uninit     = true
    };

    APP_ERROR_CHECK(nrf_twi_mngr_init(&m_nrf_twi_mngr, &config));
}

nrf_twi_mngr_t const * TwiGetMngr() {
	if (!IsTwiInitialized) {
		TwiMngrInit();
		IsTwiInitialized = true;
	}
	return &m_nrf_twi_mngr;
}

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

void pca_wr_cb(ret_code_t result, void * p_user_data) {}

// Set pin to 1
void PcaPinOn(uint8_t ch)
{
	static uint8_t pinon[] = { 0, 0, 0x10, 0, 0 };
	static nrf_twi_mngr_transfer_t transfers[] = { NRF_TWI_MNGR_WRITE(PCA9685_ADDR, pinon, 5, 0) };
    static nrf_twi_mngr_transaction_t NRF_TWI_MNGR_BUFFER_LOC_IND transaction =
    {
        .callback            = pca_wr_cb,
        .p_user_data         = NULL,
        .p_transfers         = transfers,
        .number_of_transfers = 1
    };

    pinon[0] = (ch<<2) + PCA9685_LED0_REG;
	APP_ERROR_CHECK(nrf_twi_mngr_schedule(&m_nrf_twi_mngr, &transaction));
}

// Reset pin to 0
void PcaPinOff(uint8_t ch)
{
	static uint8_t pinoff[] = { 0, 0, 0, 0, 0x10 };
	static nrf_twi_mngr_transfer_t transfers[] = { NRF_TWI_MNGR_WRITE(PCA9685_ADDR, pinoff, 5, 0) };
    static nrf_twi_mngr_transaction_t NRF_TWI_MNGR_BUFFER_LOC_IND transaction =
    {
        .callback            = pca_wr_cb,
        .p_user_data         = NULL,
        .p_transfers         = transfers,
        .number_of_transfers = 1
    };

    pinoff[0] = (ch<<2) + PCA9685_LED0_REG;
	APP_ERROR_CHECK(nrf_twi_mngr_schedule(&m_nrf_twi_mngr, &transaction));
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
			static uint8_t pinval[] = { 0, 0xFF, 0xF, 0, 0 };
			static nrf_twi_mngr_transfer_t transfers[] = { NRF_TWI_MNGR_WRITE(PCA9685_ADDR, pinval, 5, 0) };
		    static nrf_twi_mngr_transaction_t NRF_TWI_MNGR_BUFFER_LOC_IND transaction =
		    {
		        .callback            = pca_wr_cb,
		        .p_user_data         = NULL,
		        .p_transfers         = transfers,
		        .number_of_transfers = 1
		    };

		    pinval[0] = (ch == PCA9685_ALLLED_REG)? ch : (ch<<2) + PCA9685_LED0_REG;
		    pinval[3] = val_on&0xFF;
		    pinval[4] = val_on>>8;
			APP_ERROR_CHECK(nrf_twi_mngr_schedule(&m_nrf_twi_mngr, &transaction));
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
	const uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND config0[] = { PCA9685_MODE1_REG, PCA9685_MODE_SLEEP };
	const uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND config1[] = { PCA9685_PRESCALE_REG, 3 }; // max pwm freq
	const uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND config2[] = { PCA9685_MODE1_REG, PCA9685_MODE_RESTART };
	const uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND config3[] = { PCA9685_MODE1_REG, PCA9685_MODE_AUTOINC };
	const uint8_t NRF_TWI_MNGR_BUFFER_LOC_IND config4[] = { PCA9685_MODE2_REG, 0x4 };
	#define PCA9685_INIT_TRANSFER_COUNT 5

	nrf_twi_mngr_transfer_t const pca9685_init_transfers[PCA9685_INIT_TRANSFER_COUNT] =
	{
	    NRF_TWI_MNGR_WRITE(PCA9685_ADDR, config0, sizeof(config0), 0),
	    NRF_TWI_MNGR_WRITE(PCA9685_ADDR, config1, sizeof(config1), 0),
	    NRF_TWI_MNGR_WRITE(PCA9685_ADDR, config2, sizeof(config2), 0),
	    NRF_TWI_MNGR_WRITE(PCA9685_ADDR, config3, sizeof(config3), 0),
	    NRF_TWI_MNGR_WRITE(PCA9685_ADDR, config4, sizeof(config4), 0),
	};

	APP_ERROR_CHECK(nrf_twi_mngr_perform(TwiGetMngr(), NULL, pca9685_init_transfers,
        PCA9685_INIT_TRANSFER_COUNT, NULL));
}
