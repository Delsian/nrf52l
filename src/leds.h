/*
 * leds.h
 *
 *  Created on: Dec 11, 2017
 *      Author: Eug
 */

#ifndef LEDS_H_
#define LEDS_H_


typedef enum _ledsControlSignals {
	LED1_ON,
	LED1_OFF,
	LED2_ON,
	LED2_OFF,
	LED3_ON,
	LED3_OFF
} LedsControlSignal;

void leds_init(void);
void leds_scheduler(void * p_event_data, uint16_t event_size);

#endif /* LEDS_H_ */
