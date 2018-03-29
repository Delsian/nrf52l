/*
 * js_module.h
 *
 *  Created on: Mar 28, 2018
 *      Author: Eug
 */

#ifndef JS_MODULE_H_
#define JS_MODULE_H_

void JsStopScript(void);
void JsCheckApp(void);
void JsInit(void);


/*
 * Script storage area.
 * Format:
 * u16 Size
 * u32 CRC
 * u8[Size] script
 */
extern uint8_t m_script_buffer[];

#endif /* JS_MODULE_H_ */
