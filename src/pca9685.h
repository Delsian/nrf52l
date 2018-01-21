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
void PcaLed(uint8_t color);

#endif /* PCA9685_H_ */
