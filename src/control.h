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
	CE_BT_CONN,
	CE_PWR_OFF
} ControlEventType;

typedef struct {
	ControlEventType type;
	union {
		void*		ptr;
		uint8_t*	ptr8;
		uint16_t*	ptr16;
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
