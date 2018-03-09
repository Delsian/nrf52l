/*
 * r0b1c_service.h
 *
 *  Created on: 15.02.18
 *      Author: ekrashtan
 */

#ifndef R0B1C_SERVICE_H_
#define R0B1C_SERVICE_H_

void SendOverloadNotification(uint8_t *pval);
void SendButtonNotification(uint8_t *pval);
void SendBatteryNotification(const uint8_t mask);

#endif /* R0B1C_SERVICE_H_ */
