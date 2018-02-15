/*
 * r0b1c_cmd.h
 *
 *  Created on: 15.02.18
 *      Author: ekrashtan
 */

#ifndef R0B1C_CMD_H_
#define R0B1C_CMD_H_

extern tCharVars tCharCmdHandle;
extern tCharVars tCharPortHandle;
void OnPortWriteEvt(ble_evt_t const * p_ble_evt);
void OnPortNotifyEvt(ble_evt_t const * p_ble_evt);
void OnCmdWriteEvt(ble_evt_t const * p_ble_evt);

#endif /* R0B1C_CMD_H_ */
