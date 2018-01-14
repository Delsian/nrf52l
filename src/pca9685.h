/*
 * pca9685.h
 *
 *  Created on: Jan 13, 2018
 *      Author: Eug
 */

#ifndef PCA9685_H_
#define PCA9685_H_

void PcaInit(void);
void PcaWriteChannel(uint8_t ch, uint16_t val_on, uint16_t val_off);

#endif /* PCA9685_H_ */
