/*
 * pca9685.h
 *
 *  Created on: Jan 13, 2018
 *      Author: Eug
 */

#ifndef PCA9685_H_
#define PCA9685_H_

void PcaInit(void);
void PcaWriteChannel(uint8_t ch, uint8_t val);
void PcaSetLed(uint8_t r, uint8_t g, uint8_t b);
void PcaLed(uint8_t color);
void PcaPinOn(uint8_t ch);
void PcaPinOff(uint8_t ch);

#endif /* PCA9685_H_ */
