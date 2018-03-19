/*
 * jswrap_motor.h
 *
 *  Created on: Mar 19, 2018
 *      Author: Eug
 */

#ifndef DEVICES_JSWRAP_MOTOR_H_
#define DEVICES_JSWRAP_MOTOR_H_

JsVar *jswrap_motor_constructor(int port);
void jswrap_motor_write(JsVar *parent, int speed, int time);

#endif /* DEVICES_JSWRAP_MOTOR_H_ */
