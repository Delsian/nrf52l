/*
 * control.h
 *
 *  Created on: Dec 11, 2017
 *      Author: Eug
 */

#ifndef CONTROL_H_
#define CONTROL_H_

typedef enum {
	CE_BUTTON,
	CE_LED_SET, // Set new blinking pattern
	CE_BATT_IN, // Battery measurement complete
	CE_BUZZER,
	CE_PWR_OFF,
	CE_DEVNAME_CHG // Device name change
} ControlEventType;

typedef struct {
	ControlEventType type;
	union {
		void*		ptr;
		uint8_t*	ptr8;
	};
} ControlEvent;

void ControlPost(const ControlEvent* pEvt);
void ControlInit(void);

#endif /* CONTROL_H_ */
