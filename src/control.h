/*
 * control.h
 *
 *  Created on: Dec 11, 2017
 *      Author: Eug
 */

#ifndef CONTROL_H_
#define CONTROL_H_
#include <stdbool.h>

typedef enum {
	CE_BUTTON,
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
		bool b;
	};
} ControlEvent;

typedef void (*ControlEvtCb)(const ControlEvent* pEvt);

void ControlPost(const ControlEvent* pEvt);
void ControlInit(void);

// Register callback for specific event
void ControlRegisterCb(ControlEventType type, ControlEvtCb cb);
// ToDo: De-register?

#endif /* CONTROL_H_ */
