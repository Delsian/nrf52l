/*
 * control.h
 *
 *  Created on: Dec 11, 2017
 *      Author: Eug
 */

#ifndef CONTROL_H_
#define CONTROL_H_

typedef enum _controlSignals {
	BT_ADV_ON,
	BT_ADV_OFF,
	BT_CONNECT,
	BT_DISCONNECT,
} ControlSignal;

/**
 * Handler to receive task
 *
 * @param ControlSignal signal to handle
 *
 * @return pdTRUE if the signal was successfully received
 *
 */
typedef bool (ControlFunction)( ControlSignal );

void control_register_receiver(ControlFunction* pf);
void control_delete_receiver(ControlFunction* f);
void control_post_event(ControlSignal signal);
void control_init(void);

#endif /* CONTROL_H_ */
