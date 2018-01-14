/*
 * pca9685.c
 *
 *  Created on: Jan 13, 2018
 *      Author: Eug
 */

#include <stdint.h>
#include <stdio.h>
#include "pca9685.h"
#include "task_manager.h"
#include "nrf_twi_mngr.h"
#include "boards.h"
#include "nrf_delay.h"

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

NRF_TWI_MNGR_DEF(m_nrf_twi_mngr, 1, 0);
static uint8_t ubData[5];
static nrf_twi_mngr_transfer_t ptTransfers[1] =
{
	NRF_TWI_MNGR_WRITE(PCA9685_ADDR, ubData, 5, 0)
};
const static nrf_twi_mngr_transaction_t tTransaction =
{
    .callback            = NULL,
    .p_user_data         = NULL,
    .p_transfers         = ptTransfers,
    .number_of_transfers = 1
};

static bool PcaQueueFull()
{
	return nrf_queue_is_full(m_nrf_twi_mngr.p_queue);
}

// write 2 byte value using auto-increment
void PcaWriteChannel(uint8_t ch, uint16_t val_on, uint16_t val_off)
{
	// keep waiting for prev transfer
	while (PcaQueueFull()) nrf_delay_us(10);

	if (ch<=PCA9685_MAX_CHANNEL || ch == PCA9685_ALLLED_REG) {
		ubData[0] = (ch == PCA9685_ALLLED_REG)? ch : ((ch<<2) + PCA9685_LED0_REG);
		ubData[1] = val_on&0xFF;
		ubData[2] = val_on>>8;
		ubData[3] = val_off&0xFF;
		ubData[4] = val_off>>8;
		nrf_twi_mngr_schedule(&m_nrf_twi_mngr, &tTransaction);
	}
}

void PcaInit(void)
{
	uint32_t err_code;

	nrf_drv_twi_config_t const config = {
	  .scl                = TWI0_SCL,
	  .sda                = TWI0_SDA,
	  .frequency          = NRF_TWI_FREQ_100K,
	  .interrupt_priority = APP_IRQ_PRIORITY_LOWEST,
	  .clear_bus_init     = false
	};

	err_code = nrf_twi_mngr_init(&m_nrf_twi_mngr, &config);
	APP_ERROR_CHECK(err_code);

	const uint8_t ubDataReset1[2] = {PCA9685_MODE1_REG, PCA9685_MODE_AUTOINC};
	const uint8_t ubDataReset2[2] = {PCA9685_MODE2_REG, 0x4};
	const nrf_twi_mngr_transfer_t ptTrRes[] =
	{
		NRF_TWI_MNGR_WRITE(PCA9685_ADDR, ubDataReset1, 2, 0),
		NRF_TWI_MNGR_WRITE(PCA9685_ADDR, ubDataReset2, 2, 0)
	};
	const nrf_twi_mngr_transaction_t tT =
	{
	    .callback            = NULL,
	    .p_user_data         = NULL,
	    .p_transfers         = ptTrRes,
	    .number_of_transfers = 2
	};
	APP_ERROR_CHECK(nrf_twi_mngr_schedule(&m_nrf_twi_mngr, &tT));

	for (int i=0; i<2; i++)
	{
		PcaWriteChannel(i,0x1ff-i*4, 0x80);
	}
}
