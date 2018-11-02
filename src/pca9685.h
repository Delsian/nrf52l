/*
 * pca9685.h
 *
 *  Created on: Jan 13, 2018
 *      Author: Eug
 */

#ifndef PCA9685_H_
#define PCA9685_H_
#include "rdev_led.h"

void PcaInit(void);
void PcaWriteChannel(uint8_t ch, uint8_t val);
void PcaLedValue(uint8_t r, uint8_t g, uint8_t b);
void PcaLedColor(LedColor color);
void PcaPinOn(uint8_t ch);
void PcaPinOff(uint8_t ch);

void bsp_board_led_on(uint32_t led_idx);
void bsp_board_led_off(uint32_t led_idx);

#endif /* PCA9685_H_ */
