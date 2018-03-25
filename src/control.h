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
	CE_BUTTON 			= 0x0001,
	CE_BATT_IN 			= 0x0002, // Battery measurement complete
	CE_BUZZER 			= 0x0004,
	CE_BT_CONN 			= 0x0008,

	CE_PWR_OFF 			= 0x1000,
	CE_PWR_OFF_REQ		= 0x2000 // Request to power off device
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
