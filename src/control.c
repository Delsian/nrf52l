/*
 * control.c
 *
 *  Created on: Dec 11, 2017
 *      Author: Eug
 */

#include <stdint.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "control.h"

#define MAX_CONTROL_RECEIVERS 8

static TaskHandle_t  h_control_thread;     /**< Definition of BLE stack thread. */
static ControlFunction* control_receivers[MAX_CONTROL_RECEIVERS];
static QueueHandle_t ControlQueue;

static void control_thread(void * arg)
{
	ControlMessage msg;
	while(1)
	{
		if( xQueueReceive(ControlQueue, &msg, ( TickType_t ) 100))
		{
			for(int i=0; i<MAX_CONTROL_RECEIVERS; i++)
			{
				if(control_receivers[i])
				{
					if ((*control_receivers[i])(msg))
						break;
				}
			}

		}
	}
}

void control_register_receiver(ControlFunction* f)
{
	for(int i=0; i<MAX_CONTROL_RECEIVERS; i++)
	{
		if(!control_receivers[i])
		{
			control_receivers[i] = f;
			return;
		}
	}
	APP_ERROR_HANDLER(NRF_ERROR_RESOURCES);
}

void control_delete_receiver(ControlFunction* f)
{
	for(int i=0; i<MAX_CONTROL_RECEIVERS; i++)
	{
		if(control_receivers[i] == f)
		{
			control_receivers[i] = NULL;
			break;
		}
	}
}

void control_post_event(ControlMessage msg)
{
	if (xQueueSend(ControlQueue, &msg, ( TickType_t ) 0) != pdPASS)
	{
		APP_ERROR_HANDLER(NRF_ERROR_TIMEOUT);
	}
}

void control_init()
{
	ControlQueue = xQueueCreate( 10, sizeof( ControlMessage ) );
	if ( ControlQueue == 0 )
	{
		APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
	}
    if(pdPASS != xTaskCreate(control_thread, "BLE", 256, NULL, 1, &h_control_thread))
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
}
