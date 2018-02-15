/*
 * rj_port.c
 *
 *  Created on: 22.01.18
 *      Author: ekrashtan
 */


#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "rj_port.h"

const RjPortPins ExtPorts[TOTAL_RJ_PORTS] = {
// Port0
		{
				.pwm = 2,
				.in1 = 3,
				.in2 = 4,

				.logic1 = 12,
				.logic2 = 13
		},
// Port1
		{
				.pwm = 5,
				.in1 = 6,
				.in2 = 7,

				.logic1 = 14,
				.logic2 = 15
		},
// Port2
		{
				.pwm = 8,
				.in1 = 9,
				.in2 = 10,

				.logic1 = 18,
				.logic2 = 16
		},
// Port3
		{
				.pwm = 11,
				.in1 = 12,
				.in2 = 13,

				.logic1 = 19,
				.logic2 = 17
		}
};

void RjPortInit(void)
{

}

