
// Automatically generated header file for NRF52832DK
// Generated by scripts/build_platform_config.py

#ifndef _PLATFORM_CONFIG_H
#define _PLATFORM_CONFIG_H


#define PC_BOARD_ID "R0b0"
#define PC_BOARD_CHIP "NRF52832"
#define PC_BOARD_CHIP_FAMILY "NRF52"

#include "nrf.h"
#define LINKER_END_VAR _end
#define LINKER_ETEXT_VAR _etext


// SYSTICK is the counter that counts up and that we use as the real-time clock
// The smaller this is, the longer we spend in interrupts, but also the more we can sleep!
#define SYSTICK_RANGE 0x1000000 // the Maximum (it is a 24 bit counter) - on Olimexino this is about 0.6 sec
#define SYSTICKS_BEFORE_USB_DISCONNECT 2

#define DEFAULT_BUSY_PIN_INDICATOR (Pin)-1 // no indicator
#define DEFAULT_SLEEP_PIN_INDICATOR (Pin)-1 // no indicator

// When to send the message that the IO buffer is getting full
#define IOBUFFER_XOFF ((TXBUFFERMASK)*6/8)
// When to send the message that we can start receiving again
#define IOBUFFER_XON ((TXBUFFERMASK)*3/8)


#define EXTI_COUNT 7
#define RAM_TOTAL (16*1024)
#define FLASH_TOTAL (128*1024)

#define JSVAR_CACHE_SIZE                1500 // Number of JavaScript variables in RAM
#define FLASH_AVAILABLE_FOR_CODE        100000
#define FLASH_PAGE_SIZE                 4096
#define FLASH_START                     0x0

#define FLASH_SAVED_CODE_START            471040
#define FLASH_SAVED_CODE_LENGTH           12288
#define FLASH_MAGIC_LOCATION              (FLASH_SAVED_CODE_START + FLASH_SAVED_CODE_LENGTH - 4)
#define FLASH_MAGIC 0xDEADBEEF

#define USART_COUNT                          1
#define SPI_COUNT                            0
#define I2C_COUNT                            2
#define ADC_COUNT                            1
#define DAC_COUNT                            0

#define DEFAULT_CONSOLE_DEVICE              EV_SERIAL1
#define DEFAULT_CONSOLE_TX_PIN 6/* D6 */
#define DEFAULT_CONSOLE_RX_PIN 8/* D8 */
#define DEFAULT_CONSOLE_BAUDRATE 9600

#define IOBUFFERMASK 127 // (max 255) amount of items in event buffer - events take ~9 bytes each
#define TXBUFFERMASK 127 // (max 255)
#define UTILTIMERTASK_TASKS (16) // Must be power of 2 - and max 256

#define LED1_PININDEX 17/* D17 */
#define LED1_ONSTATE 1
#define LED2_PININDEX 18/* D18 */
#define LED2_ONSTATE 1
#define LED3_PININDEX 19/* D19 */
#define LED3_ONSTATE 1

// definition to avoid compilation when Pin/platform config is not defined
#define IS_PIN_USED_INTERNALLY(PIN) ((false))
#define IS_PIN_A_LED(PIN) ((false)||((PIN)==17/* D17 */)||((PIN)==18/* D18 */)||((PIN)==19/* D19 */)||((PIN)==20/* D20 */))
#define IS_PIN_A_BUTTON(PIN) ((false))

#endif // _PLATFORM_CONFIG_H

