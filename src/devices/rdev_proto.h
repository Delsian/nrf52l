/*
 * rdev_proto.h
 *
 *  Created on: Mar 19, 2018
 *      Author: Eug
 */

#ifndef DEVICES_RDEV_PROTO_H_
#define DEVICES_RDEV_PROTO_H_

RDevErrCode RDevDummyInit(uint8_t port);
RDevErrCode RDevMotorSInit(uint8_t port);
RDevErrCode RDevMotorMInit(uint8_t port);
RDevErrCode RDevMotorLInit(uint8_t port);
RDevErrCode RDevMotorDeInit(uint8_t port);
RDevErrCode RDevMotorCmd(const uint8_t* pData, uint8_t len);
RDevErrCode RDevMotorTick(uint8_t port, uint32_t time);
void RDevMotorWrite(uint8_t port, uint8_t speed, uint8_t time);
RDevErrCode RDevButtonInit(uint8_t port);
RDevErrCode RDevButtonTick(uint8_t port, uint32_t time);
RDevErrCode RDevButtonCmd(const uint8_t* pData, uint8_t len);
RDevErrCode RDevRangeInit(uint8_t port);
RDevErrCode RDevRangeUnInit(uint8_t port);
RDevErrCode RDevRangeTick(uint8_t port, uint32_t time);
RDevErrCode RDevRangeCmd(const uint8_t* pData, uint8_t len);
void RDevRangeScanEnable(bool enable);
int RDevRangeGet(uint8_t port);
RDevErrCode RDevLedInit(uint8_t port);
RDevErrCode RDevLedCmd(const uint8_t* pData, uint8_t len);
RDevErrCode RDevLedTick(uint8_t port, uint32_t time);
RDevErrCode RDevGyroInit(uint8_t port);
RDevErrCode RDevGyroTick(uint8_t port, uint32_t time);
RDevErrCode RDevGyroCmd(const uint8_t* pData, uint8_t len);
RDevErrCode BatteryTick(uint8_t port, uint32_t time);
RDevErrCode RDevBattCmd(const uint8_t* pData, uint8_t len);
RDevErrCode BatteryInit(uint8_t port);
bool BatteryIfPwrOffEnabled(void);

#endif /* DEVICES_RDEV_PROTO_H_ */
