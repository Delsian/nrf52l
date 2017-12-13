/*
 * control.h
 *
 *  Created on: Dec 11, 2017
 *      Author: Eug
 */

#ifndef CONTROL_H_
#define CONTROL_H_

typedef enum _controlSignals {
	BT_ADVERT,
	BT_CONNECT,
	BT_UART_RX
} ControlSignal;

typedef struct _controlMessage {
	ControlSignal type;
	union {
		bool b;
		uint8_t* ptr;
	};
} ControlMessage;

/**
 * Handler to receive task
 *
 * @param ControlSignal signal to handle
 *
 * @return pdTRUE if the signal was successfully received
 *
 */
typedef bool (ControlFunction)( ControlMessage );

void control_register_receiver(ControlFunction* pf);
void control_delete_receiver(ControlFunction* f);
void control_post_event(ControlMessage signal);
void control_init(void);

#endif /* CONTROL_H_ */
