/*
 * Espruino related files
 *
 */


#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "bluetooth.h"
#include "jsinteractive.h"
#include "jshardware.h"
#include "jstimer.h"
#include "jsutils.h"
#include "jsparse.h"
#include "jswrap_io.h"
#include "jswrap_flash.h"

#include "cmsis_gcc.h"
#include "nrf_gpio.h"
#include "nrf_timer.h"
#include "nrf_saadc.h"
#include "nrf_pwm.h"
#include "nrf_drv_twi.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "app_timer.h"

#define SYSCLK_FREQ 32768 // this really needs to be a bit higher :)
#define NRF_TIMER_FREQ 1000000
#define NRF_TIMER_MAX 0xFFFFFFFF

BITFIELD_DECL(jshPinSoftPWM, JSH_PIN_COUNT);
bool uartInitialised = false;
JshPinFunction pinStates[JSH_PIN_COUNT];
APP_TIMER_DEF(m_wakeup_timer_id);

volatile JsSysTime baseSystemTime = 0;
volatile uint32_t lastSystemTime = 0;
volatile bool hadEvent = false; // set if we've had an event we need to deal with

#if SPI_ENABLED
static const nrf_drv_spi_t spi0 = NRF_DRV_SPI_INSTANCE(0);
bool spi0Initialised = false;
#endif

static const nrf_drv_twi_t TWI1 = NRF_DRV_TWI_INSTANCE(1);
bool twi1Initialised = false;

void esp_init()
{
	jsiConsolePrintf("Hello\n");
	jsiLoop();
}

void jshInit() {
}

void jshInterruptOff() {
  __set_BASEPRI(4<<5);
}

void jshInterruptOn() {
  __set_BASEPRI(0);
}

bool jshIsInInterrupt() {
  return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0;
}

NRF_PWM_Type *nrf_get_pwm(JshPinFunction func) {
  if ((func&JSH_MASK_TYPE) == JSH_TIMER1) return NRF_PWM0;
  else if ((func&JSH_MASK_TYPE) == JSH_TIMER2) return NRF_PWM1;
  else if ((func&JSH_MASK_TYPE) == JSH_TIMER3) return NRF_PWM2;
  return 0;
}

/// Given a Pin, return the current pin function associated with it
JshPinFunction jshGetCurrentPinFunction(Pin pin) {
  return JSH_NOTHING;
}

static IOEventFlags jshGetEventFlagsForWatchedPin(nrf_drv_gpiote_pin_t pin) {
  uint32_t addr = nrf_drv_gpiote_in_event_addr_get(pin);

  // sigh. all because the right stuff isn't exported. All we wanted was channel_port_get
  int i;
  for (i=0;i<GPIOTE_CH_NUM;i++)
    if (addr == nrf_gpiote_event_addr_get((nrf_gpiote_events_t)((uint32_t)NRF_GPIOTE_EVENTS_IN_0+(sizeof(uint32_t)*i))))
      return EV_EXTI0+i;
  return EV_NONE;
}

///< Can the given pin be watched? it may not be possible because of conflicts
bool jshCanWatch(Pin pin) {
  return true;
}

// stuff to do on idle
void jshIdle() {
}

/// Get this IC's serial number. Passed max # of chars and a pointer to write to. Returns # of chars
int jshGetSerialNumber(unsigned char *data, int maxChars) {
    memcpy(data, (void*)NRF_FICR->DEVICEID, sizeof(NRF_FICR->DEVICEID));
    return sizeof(NRF_FICR->DEVICEID);
}

bool jshIsEventForPin(IOEvent *event, Pin pin) {
  return IOEVENTFLAGS_GETTYPE(event->flags) == jshGetEventFlagsForWatchedPin((uint32_t)pinInfo[pin].pin);
}

static NO_INLINE void jshPinSetFunction(Pin pin, JshPinFunction func) {
  if (pinStates[pin]==func) return;
  // disconnect existing peripheral (if there was one)
  if (pinStates[pin])
    jshPinSetFunction_int(pinStates[pin], 0xFFFFFFFF);
  // connect new peripheral
  pinStates[pin] = func;
  jshPinSetFunction_int(pinStates[pin], pinInfo[pin].pin);
}

JshPinFunction jshGetFreeTimer(JsVarFloat freq) {
  int timer, channel, pin;
  for (timer=0;timer<3;timer++) {
    bool timerUsed = false;
    JshPinFunction timerFunc = JSH_TIMER1 + (JSH_TIMER2-JSH_TIMER1)*timer;
    if (freq>0) {
      // TODO: we could see if the frequency matches?
      // if frequency specified then if timer is used by
      // anything else we'll skip it
      for (pin=0;pin<JSH_PIN_COUNT;pin++)
        if ((pinStates[pin]&JSH_MASK_TYPE) == timerFunc)
          timerUsed = true;
    }
    if (!timerUsed) {
      // now check each channel
      for (channel=0;channel<4;channel++) {
        JshPinFunction func = timerFunc | (JSH_TIMER_CH1 + (JSH_TIMER_CH2-JSH_TIMER_CH1)*channel);
        bool timerUsed = false;
        for (pin=0;pin<JSH_PIN_COUNT;pin++)
          if ((pinStates[pin]&(JSH_MASK_TYPE|JSH_MASK_TIMER_CH)) == func)
            timerUsed = true;
        if (!timerUsed)
          return func;
      }
    }
  }
  return JSH_NOTHING;
}

JshPinFunction jshPinAnalogOutput(Pin pin, JsVarFloat value, JsVarFloat freq, JshAnalogOutputFlags flags) {
  if (pinInfo[pin].port & JSH_PIN_NEGATED)
    value = 1-value;
  // Try and use existing pin function
  JshPinFunction func = pinStates[pin];
  // If it's not a timer, try and find one
  if (!JSH_PINFUNCTION_IS_TIMER(func)) {
    func = jshGetFreeTimer(freq);
  }
  /* we set the bit field here so that if the user changes the pin state
   * later on, we can get rid of the IRQs */
  if ((flags & JSAOF_FORCE_SOFTWARE) ||
      ((flags & JSAOF_ALLOW_SOFTWARE) && !func)) {
    if (!jshGetPinStateIsManual(pin)) {
      BITFIELD_SET(jshPinSoftPWM, pin, 0);
      jshPinSetState(pin, JSHPINSTATE_GPIO_OUT);
    }
    BITFIELD_SET(jshPinSoftPWM, pin, 1);
    if (freq<=0) freq=50;
    jstPinPWM(freq, value, pin);
    return JSH_NOTHING;
  }

  if (!func) {
    jsExceptionHere(JSET_ERROR, "No free Hardware PWMs. Try not specifying a frequency, or using analogWrite(pin, val, {soft:true}) for Software PWM\n");
    return 0;
  }

  NRF_PWM_Type *pwm = nrf_get_pwm(func);
  if (!pwm) { assert(0); return 0; };
  jshPinSetState(pin, JSHPINSTATE_GPIO_OUT);
  jshPinSetFunction(pin, func);
  nrf_pwm_enable(pwm);

  nrf_pwm_clk_t clk;
  if (freq<=0) freq = 1000;
  int counter = (int)(16000000.0 / freq);

  if (counter<32768) {
    clk = NRF_PWM_CLK_16MHz;
    if (counter<1) counter=1;
  } else if (counter < (32768<<1)) {
    clk = NRF_PWM_CLK_8MHz;
    counter >>= 1;
  } else if (counter < (32768<<2)) {
    clk = NRF_PWM_CLK_4MHz;
    counter >>= 2;
  } else if (counter < (32768<<3)) {
    clk = NRF_PWM_CLK_2MHz;
    counter >>= 3;
  } else if (counter < (32768<<4)) {
    clk = NRF_PWM_CLK_1MHz;
    counter >>= 4;
  } else if (counter < (32768<<5)) {
    clk = NRF_PWM_CLK_500kHz;
    counter >>= 5;
  } else if (counter < (32768<<6)) {
    clk = NRF_PWM_CLK_250kHz;
    counter >>= 6;
  } else {
    clk = NRF_PWM_CLK_125kHz;
    counter >>= 7;
    if (counter>32767) counter = 32767;
    // Warn that we're out of range?
  }

  nrf_pwm_configure(pwm,
      clk, NRF_PWM_MODE_UP, counter /* top value - 15 bits, not 16! */);
  nrf_pwm_decoder_set(pwm,
      NRF_PWM_LOAD_INDIVIDUAL, // allow all 4 channels to be used
      NRF_PWM_STEP_TRIGGERED); // Only step on NEXTSTEP task

  /*nrf_pwm_shorts_set(pwm, 0);
  nrf_pwm_int_set(pwm, 0);
  nrf_pwm_event_clear(pwm, NRF_PWM_EVENT_LOOPSDONE);
  nrf_pwm_event_clear(pwm, NRF_PWM_EVENT_SEQEND0);
  nrf_pwm_event_clear(pwm, NRF_PWM_EVENT_SEQEND1);
  nrf_pwm_event_clear(pwm, NRF_PWM_EVENT_STOPPED);
  nrf_pwm_event_clear(pwm, NRF_PWM_EVENT_STOPPED);*/

  int timer = ((func&JSH_MASK_TYPE)-JSH_TIMER1) >> JSH_SHIFT_TYPE;
  int channel = (func&JSH_MASK_INFO) >> JSH_SHIFT_INFO;

  static uint16_t pwmValues[3][4];
  pwmValues[timer][channel] = counter - (uint16_t)(value*counter);
  nrf_pwm_loop_set(pwm, PWM_LOOP_CNT_Disabled);
  nrf_pwm_seq_ptr_set(      pwm, 0, &pwmValues[timer][0]);
  nrf_pwm_seq_cnt_set(      pwm, 0, 4);
  nrf_pwm_seq_refresh_set(  pwm, 0, 0);
  nrf_pwm_seq_end_delay_set(pwm, 0, 0);

  nrf_pwm_task_trigger(pwm, NRF_PWM_TASK_SEQSTART0);
  //nrf_pwm_disable(pwm);
  return func;
} // if freq<=0, the default is used

bool lastHandledPinState; ///< bit of a hack, this... Ideally get rid of WatchedPinState completely and add to jshPushIOWatchEvent
static void jsvPinWatchHandler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
  lastHandledPinState = (bool)nrf_gpio_pin_read(pin);
  IOEventFlags evt = jshGetEventFlagsForWatchedPin(pin);
  jshPushIOWatchEvent(evt);
}

IOEventFlags jshPinWatch(Pin pin, bool shouldWatch) {
  if (!jshIsPinValid(pin)) return EV_NONE;
  uint32_t p = (uint32_t)pinInfo[pin].pin;
  if (shouldWatch) {
    nrf_drv_gpiote_in_config_t cls_1_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true); // FIXME: Maybe we want low accuracy? Otherwise this will
    cls_1_config.is_watcher = true; // stop this resetting the input state
    nrf_drv_gpiote_in_init(p, &cls_1_config, jsvPinWatchHandler);
    nrf_drv_gpiote_in_event_enable(p, true);
    return jshGetEventFlagsForWatchedPin(p);
  } else {
    nrf_drv_gpiote_in_event_disable(p);
    return EV_NONE;
  }
} // start watching pin - return the EXTI associated with it

void jshPinSetFunction_int(JshPinFunction func, uint32_t pin) {
  JshPinFunction fType = func&JSH_MASK_TYPE;
  JshPinFunction fInfo = func&JSH_MASK_INFO;
  switch (fType) {
  case JSH_NOTHING: break;
  case JSH_TIMER1:
  case JSH_TIMER2:
  case JSH_TIMER3: {
      NRF_PWM_Type *pwm = nrf_get_pwm(fType);
      pwm->PSEL.OUT[fInfo>>JSH_SHIFT_INFO] = pin;
      // FIXME: Only disable if nothing else is using it!
      if (pin==0xFFFFFFFF) nrf_pwm_disable(pwm);
      break;
    }
  case JSH_USART1: if (fInfo==JSH_USART_RX) NRF_UART0->PSELRXD = pin;
                   else NRF_UART0->PSELTXD = pin;
                   // TODO: do we need to disable the UART driver if both pins are undefined?
                   break;
#if SPI_ENABLED
  case JSH_SPI1: if (fInfo==JSH_SPI_MISO) NRF_SPI0->PSELMISO = pin;
                 else if (fInfo==JSH_SPI_MOSI) NRF_SPI0->PSELMOSI = pin;
                 else NRF_SPI0->PSELSCK = pin;
                 break;
#endif
  case JSH_I2C1: if (fInfo==JSH_I2C_SDA) NRF_TWI1->PSELSDA = pin;
                 else NRF_TWI1->PSELSCL = pin;
                 break;
  default: assert(0);
  }
}


unsigned int jshSetSystemClock(JsVar *options) {
  return 0;
}

/// Given a pin function, set that pin to the 16 bit value (used mainly for DACs and PWM)
void jshSetOutputValue(JshPinFunction func, int value) {
}

/** Check the pin associated with this EXTI - return true if it is a 1 */
bool jshGetWatchedPinState(IOEventFlags device) {
  return lastHandledPinState;
}

static nrf_saadc_value_t nrf_analog_read() {
  nrf_saadc_value_t result;
  nrf_saadc_buffer_init(&result,1);

  nrf_saadc_task_trigger(NRF_SAADC_TASK_START);

  while(!nrf_saadc_event_check(NRF_SAADC_EVENT_STARTED));
  nrf_saadc_event_clear(NRF_SAADC_EVENT_STARTED);

  nrf_saadc_task_trigger(NRF_SAADC_TASK_SAMPLE);


  while(!nrf_saadc_event_check(NRF_SAADC_EVENT_END));
  nrf_saadc_event_clear(NRF_SAADC_EVENT_END);

  nrf_saadc_task_trigger(NRF_SAADC_TASK_STOP);
  while(!nrf_saadc_event_check(NRF_SAADC_EVENT_STOPPED));
  nrf_saadc_event_clear(NRF_SAADC_EVENT_STOPPED);

  return result;
}

/// Returns a quickly-read analog value in the range 0-65535
int jshPinAnalogFast(Pin pin) {
  if (pinInfo[pin].analog == JSH_ANALOG_NONE) return 0;

  // sanity checks for channel
  assert(NRF_SAADC_INPUT_AIN0 == 1);
  assert(NRF_SAADC_INPUT_AIN1 == 2);
  assert(NRF_SAADC_INPUT_AIN2 == 3);
  nrf_saadc_input_t ain = 1 + (pinInfo[pin].analog & JSH_MASK_ANALOG_CH);

  nrf_saadc_channel_config_t config;
  config.acq_time = NRF_SAADC_ACQTIME_3US;
  config.gain = NRF_SAADC_GAIN1_4; // 1/4 of input volts
  config.mode = NRF_SAADC_MODE_SINGLE_ENDED;
  config.pin_p = ain;
  config.pin_n = ain;
  config.reference = NRF_SAADC_REFERENCE_VDD4; // VDD/4 as reference.
  config.resistor_p = NRF_SAADC_RESISTOR_DISABLED;
  config.resistor_n = NRF_SAADC_RESISTOR_DISABLED;

  // make reading
  nrf_saadc_enable();
  nrf_saadc_resolution_set(NRF_SAADC_RESOLUTION_8BIT);
  nrf_saadc_channel_init(0, &config);

  return nrf_analog_read() << 8;
}

/** Is the given device initialised? */
bool jshIsDeviceInitialised(IOEventFlags device) {
#if SPI_ENABLED
  if (device==EV_SPI1) return spi0Initialised;
#endif
  if (device==EV_I2C1) return twi1Initialised;
  if (device==EV_SERIAL1) return uartInitialised;
  return false;
}

/// Get the system time (in ticks)
JsSysTime jshGetSystemTime() {
  // Detect RTC overflows
  uint32_t systemTime = NRF_RTC0->COUNTER;
  if ((lastSystemTime & 0x800000) && !(systemTime & 0x800000))
    baseSystemTime += 0x1000000; // it's a 24 bit counter
  lastSystemTime = systemTime;
  // Use RTC0 (also used by BLE stack) - as app_timer starts/stops RTC1
  return baseSystemTime + (JsSysTime)systemTime;
}

/// Set the system time (in ticks) - this should only be called rarely as it could mess up things like jsinteractive's timers!
void jshSetSystemTime(JsSysTime time) {
  baseSystemTime = 0;
  baseSystemTime = time - jshGetSystemTime();
}

/// Convert a time in Milliseconds to one in ticks.
JsSysTime jshGetTimeFromMilliseconds(JsVarFloat ms) {
  return (JsSysTime) ((ms * SYSCLK_FREQ) / 1000);
}

/// Convert ticks to a time in Milliseconds.
JsVarFloat jshGetMillisecondsFromTime(JsSysTime time) {
  return (time * 1000.0) / SYSCLK_FREQ;
}


void jshPinSetValue(Pin pin, bool value) {
  if (pinInfo[pin].port & JSH_PIN_NEGATED) value=!value;
  nrf_gpio_pin_write((uint32_t)pinInfo[pin].pin, value);
}

bool jshPinGetValue(Pin pin) {
  bool value = nrf_gpio_pin_read((uint32_t)pinInfo[pin].pin);
  if (pinInfo[pin].port & JSH_PIN_NEGATED) value=!value;
  return value;
}

/** Get the pin state (only accurate for simple IO - won't return JSHPINSTATE_USART_OUT for instance).
 * Note that you should use JSHPINSTATE_MASK as other flags may have been added */
JshPinState jshPinGetState(Pin pin) {
  assert(jshIsPinValid(pin));
  uint32_t ipin = (uint32_t)pinInfo[pin].pin;
  uint32_t p = NRF_GPIO->PIN_CNF[ipin];
  if ((p&GPIO_PIN_CNF_DIR_Msk)==(GPIO_PIN_CNF_DIR_Output<<GPIO_PIN_CNF_DIR_Pos)) {
    // Output
    JshPinState hi = (NRF_GPIO->OUT & (1<<ipin)) ? JSHPINSTATE_PIN_IS_ON : 0;
    if ((p&GPIO_PIN_CNF_DRIVE_Msk)==(GPIO_PIN_CNF_DRIVE_S0D1<<GPIO_PIN_CNF_DRIVE_Pos)) {
      if ((p&GPIO_PIN_CNF_PULL_Msk)==(GPIO_PIN_CNF_PULL_Pullup<<GPIO_PIN_CNF_PULL_Pos))
        return JSHPINSTATE_GPIO_OUT_OPENDRAIN_PULLUP|hi;
      else {
        if (pinStates[pin])
          return JSHPINSTATE_AF_OUT_OPENDRAIN|hi;
        else
          return JSHPINSTATE_GPIO_OUT_OPENDRAIN|hi;
      }
    } else {
      if (pinStates[pin])
        return JSHPINSTATE_AF_OUT|hi;
      else
        return JSHPINSTATE_GPIO_OUT|hi;
    }
  } else {
    // Input
    if ((p&GPIO_PIN_CNF_PULL_Msk)==(GPIO_PIN_CNF_PULL_Pullup<<GPIO_PIN_CNF_PULL_Pos)) {
      return JSHPINSTATE_GPIO_IN_PULLUP;
    } else if ((p&GPIO_PIN_CNF_PULL_Msk)==(GPIO_PIN_CNF_PULL_Pulldown<<GPIO_PIN_CNF_PULL_Pos)) {
      return JSHPINSTATE_GPIO_IN_PULLDOWN;
    } else {
      return JSHPINSTATE_GPIO_IN;
    }
  }
}

void jshPinSetState(Pin pin, JshPinState state) {
  assert(jshIsPinValid(pin));
  // If this was set to be some kind of AF (USART, etc), reset it.
  jshPinSetFunction(pin, JSH_NOTHING);
  /* Make sure we kill software PWM if we set the pin state
   * after we've started it */
  if (BITFIELD_GET(jshPinSoftPWM, pin)) {
    BITFIELD_SET(jshPinSoftPWM, pin, 0);
    jstPinPWM(0,0,pin);
  }
  if (pinInfo[pin].port & JSH_PIN_NEGATED) {
    if (state==JSHPINSTATE_GPIO_IN_PULLUP) state=JSHPINSTATE_GPIO_IN_PULLDOWN;
    else if (state==JSHPINSTATE_GPIO_IN_PULLDOWN) state=JSHPINSTATE_GPIO_IN_PULLUP;
  }

  uint32_t ipin = (uint32_t)pinInfo[pin].pin;
  switch (state) {
    case JSHPINSTATE_UNDEFINED :
      NRF_GPIO->PIN_CNF[ipin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                              | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                              | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                              | (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
                              | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
      break;
    case JSHPINSTATE_AF_OUT :
    case JSHPINSTATE_GPIO_OUT :
    case JSHPINSTATE_USART_OUT :
      NRF_GPIO->PIN_CNF[ipin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                              | (GPIO_PIN_CNF_DRIVE_H0H1 << GPIO_PIN_CNF_DRIVE_Pos)
                              | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                              | (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
                              | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
      break;
    case JSHPINSTATE_AF_OUT_OPENDRAIN :
    case JSHPINSTATE_GPIO_OUT_OPENDRAIN :
      NRF_GPIO->PIN_CNF[ipin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                              | (GPIO_PIN_CNF_DRIVE_H0D1 << GPIO_PIN_CNF_DRIVE_Pos)
                              | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                              | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                              | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
      break;
    case JSHPINSTATE_I2C :
    case JSHPINSTATE_GPIO_OUT_OPENDRAIN_PULLUP:
      NRF_GPIO->PIN_CNF[ipin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                              | (GPIO_PIN_CNF_DRIVE_H0D1 << GPIO_PIN_CNF_DRIVE_Pos)
                              | (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)
                              | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                              | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
      break;
    case JSHPINSTATE_GPIO_IN :
    case JSHPINSTATE_ADC_IN :
    case JSHPINSTATE_USART_IN :
      nrf_gpio_cfg_input(ipin, NRF_GPIO_PIN_NOPULL);
      break;
    case JSHPINSTATE_GPIO_IN_PULLUP :
      nrf_gpio_cfg_input(ipin, NRF_GPIO_PIN_PULLUP);
      break;
    case JSHPINSTATE_GPIO_IN_PULLDOWN :
      nrf_gpio_cfg_input(ipin, NRF_GPIO_PIN_PULLDOWN);
      break;
    default : jsiConsolePrintf("Unimplemented pin state %d\n", state);
      break;
  }
}

void jshPinPulse(Pin pin, bool pulsePolarity, JsVarFloat pulseTime) {
  // ---- USE TIMER FOR PULSE
  if (!jshIsPinValid(pin)) {
       jsExceptionHere(JSET_ERROR, "Invalid pin!");
       return;
  }
  if (pulseTime<=0) {
    // just wait for everything to complete
    jstUtilTimerWaitEmpty();
    return;
  } else {
    // find out if we already had a timer scheduled
    UtilTimerTask task;
    if (!jstGetLastPinTimerTask(pin, &task)) {
      // no timer - just start the pulse now!
      jshPinOutput(pin, pulsePolarity);
      task.time = jshGetSystemTime();
    }
    // Now set the end of the pulse to happen on a timer
    jstPinOutputAtTime(task.time + jshGetTimeFromMilliseconds(pulseTime), &pin, 1, !pulsePolarity);
  }
}


/**
 * Get a random number - either using special purpose hardware or by
 * reading noise from an analog input. If unimplemented, this should
 * default to `rand()`
 */
unsigned int jshGetRandomNumber() {
  unsigned int v = 0;
  uint8_t bytes_avail = 0;
  WAIT_UNTIL((sd_rand_application_bytes_available_get(&bytes_avail),bytes_avail>=sizeof(v)),"Random number");
  sd_rand_application_vector_get((uint8_t*)&v, sizeof(v));
  return v;
}

// Returns an analog value between 0 and 1
JsVarFloat jshPinAnalog(Pin pin) {
  if (pinInfo[pin].analog == JSH_ANALOG_NONE) return NAN;
  if (!jshGetPinStateIsManual(pin))
    jshPinSetState(pin, JSHPINSTATE_ADC_IN);
  // sanity checks for channel
  assert(NRF_SAADC_INPUT_AIN0 == 1);
  assert(NRF_SAADC_INPUT_AIN1 == 2);
  assert(NRF_SAADC_INPUT_AIN2 == 3);
  nrf_saadc_input_t ain = 1 + (pinInfo[pin].analog & JSH_MASK_ANALOG_CH);

  nrf_saadc_channel_config_t config;
  config.acq_time = NRF_SAADC_ACQTIME_3US;
  config.gain = NRF_SAADC_GAIN1_4; // 1/4 of input volts
  config.mode = NRF_SAADC_MODE_SINGLE_ENDED;
  config.pin_p = ain;
  config.pin_n = ain;
  config.reference = NRF_SAADC_REFERENCE_VDD4; // VDD/4 as reference.
  config.resistor_p = NRF_SAADC_RESISTOR_DISABLED;
  config.resistor_n = NRF_SAADC_RESISTOR_DISABLED;

  // make reading
  nrf_saadc_enable();
  nrf_saadc_resolution_set(NRF_SAADC_RESOLUTION_14BIT);
  nrf_saadc_channel_init(0, &config);

  JsVarFloat f = nrf_analog_read() / 16384.0;
  nrf_saadc_channel_input_set(0, NRF_SAADC_INPUT_DISABLED, NRF_SAADC_INPUT_DISABLED); // give us back our pin!
  nrf_saadc_disable();
  return f;
}

void jshUSARTSetup(IOEventFlags device, JshUSARTInfo *inf) {
	// ToDo  RTT output
    uartInitialised = true;
}

void jshUSARTKick(IOEventFlags device) {
}

void jshReset() {
}

/// Enter simple sleep mode (can be woken up by interrupts). Returns true on success
bool jshSleep(JsSysTime timeUntilWake) {
  /* Wake ourselves up if we're supposed to, otherwise if we're not waiting for
   any particular time, just sleep. */
  /* Wake up minimum every 4 minutes, to ensure that we notice if the
   * RTC is going to overflow. On nRF51 we can only easily use RTC0 for time
   * (RTC1 gets started and stopped by app timer), and we can't get an IRQ
   * when it overflows, so we'll have to check for overflows (which means always
   * waking up with enough time to detect an overflow).
   */
  if (timeUntilWake > jshGetTimeFromMilliseconds(240*1000))
    timeUntilWake = jshGetTimeFromMilliseconds(240*1000);
  if (timeUntilWake < JSSYSTIME_MAX) {
    uint32_t ticks = APP_TIMER_TICKS(jshGetMillisecondsFromTime(timeUntilWake));
    if (ticks<APP_TIMER_MIN_TIMEOUT_TICKS) return false; // can't sleep this short an amount of time
    uint32_t err_code = app_timer_start(m_wakeup_timer_id, ticks, NULL);
    if (err_code) jsiConsolePrintf("app_timer_start error %d\n", err_code);
  }
  hadEvent = false;
  jsiSetSleep(JSI_SLEEP_ASLEEP);
  while (!hadEvent) {
    sd_app_evt_wait(); // Go to sleep, wait to be woken up
    jshGetSystemTime(); // check for RTC overflows
  }
  jsiSetSleep(JSI_SLEEP_AWAKE);
  // we don't care about the return codes...
  app_timer_stop(m_wakeup_timer_id);
  return true;
}

/** Set up SPI, if pins are -1 they will be guessed */
void jshSPISetup(IOEventFlags device, JshSPIInfo *inf) {
#if SPI_ENABLED
  if (device!=EV_SPI1) return;

  nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;

  nrf_spi_frequency_t freq;
  if (inf->baudRate<((125000+250000)/2))
    freq = SPI_FREQUENCY_FREQUENCY_K125;
  else if (inf->baudRate<((250000+500000)/2))
    freq = SPI_FREQUENCY_FREQUENCY_K250;
  else if (inf->baudRate<((500000+1000000)/2))
    freq = SPI_FREQUENCY_FREQUENCY_K500;
  else if (inf->baudRate<((1000000+2000000)/2))
    freq = SPI_FREQUENCY_FREQUENCY_M1;
  else if (inf->baudRate<((2000000+4000000)/2))
    freq = SPI_FREQUENCY_FREQUENCY_M2;
  else if (inf->baudRate<((4000000+8000000)/2))
    freq = SPI_FREQUENCY_FREQUENCY_M4;
  else
    freq = SPI_FREQUENCY_FREQUENCY_M8;
  spi_config.frequency =  freq;
  spi_config.mode = inf->spiMode;
  spi_config.bit_order = inf->spiMSB ? NRF_DRV_SPI_BIT_ORDER_MSB_FIRST : NRF_DRV_SPI_BIT_ORDER_LSB_FIRST;
  if (jshIsPinValid(inf->pinSCK))
    spi_config.sck_pin = (uint32_t)pinInfo[inf->pinSCK].pin;
  if (jshIsPinValid(inf->pinMISO))
    spi_config.miso_pin = (uint32_t)pinInfo[inf->pinMISO].pin;
  if (jshIsPinValid(inf->pinMOSI))
    spi_config.mosi_pin = (uint32_t)pinInfo[inf->pinMOSI].pin;

  if (spi0Initialised) nrf_drv_spi_uninit(&spi0);
  spi0Initialised = true;
  // No event handler means SPI transfers are blocking
  uint32_t err_code = nrf_drv_spi_init(&spi0, &spi_config, NULL);
  if (err_code != NRF_SUCCESS)
    jsExceptionHere(JSET_INTERNALERROR, "SPI Initialisation Error %d\n", err_code);

  // nrf_drv_spi_init will set pins, but this ensures we know so can reset state later
  if (jshIsPinValid(inf->pinSCK)) {
    jshPinSetFunction(inf->pinSCK, JSH_SPI1|JSH_SPI_SCK);
  }
  if (jshIsPinValid(inf->pinMOSI)) {
    jshPinSetFunction(inf->pinMOSI, JSH_SPI1|JSH_SPI_MOSI);
  }
  if (jshIsPinValid(inf->pinMISO)) {
    jshPinSetFunction(inf->pinMISO, JSH_SPI1|JSH_SPI_MISO);
  }
#endif
}

/** Send data through the given SPI device (if data>=0), and return the result
 * of the previous send (or -1). If data<0, no data is sent and the function
 * waits for data to be returned */
int jshSPISend(IOEventFlags device, int data) {
#if SPI_ENABLED
  if (device!=EV_SPI1) return -1;
  uint8_t tx = (uint8_t)data;
  uint8_t rx = 0;
  uint32_t err_code = nrf_drv_spi_transfer(&spi0, &tx, 1, &rx, 1);
  if (err_code != NRF_SUCCESS)
    jsExceptionHere(JSET_INTERNALERROR, "SPI Send Error %d\n", err_code);
  return rx;
#endif
}

/** Send 16 bit data through the given SPI device. */
void jshSPISend16(IOEventFlags device, int data) {
#if SPI_ENABLED
  if (device!=EV_SPI1) return;
  uint16_t tx = (uint16_t)data;
  uint32_t err_code = nrf_drv_spi_transfer(&spi0, (uint8_t*)&tx, 1, 0, 0);
  if (err_code != NRF_SUCCESS)
    jsExceptionHere(JSET_INTERNALERROR, "SPI Send Error %d\n", err_code);
#endif
}

/** Set whether to send 16 bits or 8 over SPI */
void jshSPISet16(IOEventFlags device, bool is16) {
}

/** Set whether to use the receive interrupt or not */
void jshSPISetReceive(IOEventFlags device, bool isReceive) {
}

/** Wait until SPI send is finished, and flush all received data */
void jshSPIWait(IOEventFlags device) {
}


// the temperature from the internal temperature sensor
JsVarFloat jshReadTemperature() {
  /* Softdevice makes us fault - we must access
  this via the function */
  int32_t temp;
  uint32_t err_code = sd_temp_get(&temp);
  if (err_code) return NAN;
  return temp/4.0;
}

// The voltage that a reading of 1 from `analogRead` actually represents
JsVarFloat jshReadVRef() {
  nrf_saadc_channel_config_t config;
  config.acq_time = NRF_SAADC_ACQTIME_3US;
  config.gain = NRF_SAADC_GAIN1_6; // 1/6 of input volts
  config.mode = NRF_SAADC_MODE_SINGLE_ENDED;
  config.pin_p = NRF_SAADC_INPUT_VDD;
  config.pin_n = NRF_SAADC_INPUT_VDD;
  config.reference = NRF_SAADC_REFERENCE_INTERNAL; // 0.6v reference.
  config.resistor_p = NRF_SAADC_RESISTOR_DISABLED;
  config.resistor_n = NRF_SAADC_RESISTOR_DISABLED;

  // make reading
  nrf_saadc_enable();
  nrf_saadc_resolution_set(NRF_SAADC_RESOLUTION_14BIT);
  nrf_saadc_channel_init(0, &config);

  return 6.0 * (nrf_analog_read() * 0.6 / 16384.0);
}

const nrf_drv_twi_t *jshGetTWI(IOEventFlags device) {
  if (device == EV_I2C1) return &TWI1;
  return 0;
}

/** Set up I2C, if pins are -1 they will be guessed */
void jshI2CSetup(IOEventFlags device, JshI2CInfo *inf) {
  if (!jshIsPinValid(inf->pinSCL) || !jshIsPinValid(inf->pinSDA)) {
    jsError("SDA and SCL pins must be valid, got %d and %d\n", inf->pinSDA, inf->pinSCL);
    return;
  }
  const nrf_drv_twi_t *twi = jshGetTWI(device);
  if (!twi) return;
  // http://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk51.v9.0.0%2Fhardware_driver_twi.html&cp=4_1_0_2_10
  nrf_drv_twi_config_t    p_twi_config;
  p_twi_config.scl = (uint32_t)pinInfo[inf->pinSCL].pin;
  p_twi_config.sda = (uint32_t)pinInfo[inf->pinSDA].pin;
  p_twi_config.frequency = (inf->bitrate<175000) ? NRF_TWI_FREQ_100K : ((inf->bitrate<325000) ? NRF_TWI_FREQ_250K : NRF_TWI_FREQ_400K);
  p_twi_config.interrupt_priority = APP_IRQ_PRIORITY_LOW;
  if (twi1Initialised) nrf_drv_twi_uninit(twi);
  twi1Initialised = true;
  uint32_t err_code = nrf_drv_twi_init(twi, &p_twi_config, NULL, NULL);
  if (err_code != NRF_SUCCESS)
    jsExceptionHere(JSET_INTERNALERROR, "I2C Initialisation Error %d\n", err_code);
  else
    nrf_drv_twi_enable(twi);

  // nrf_drv_spi_init will set pins, but this ensures we know so can reset state later
  if (jshIsPinValid(inf->pinSCL)) {
    jshPinSetFunction(inf->pinSCL, JSH_I2C1|JSH_I2C_SCL);
  }
  if (jshIsPinValid(inf->pinSDA)) {
    jshPinSetFunction(inf->pinSDA, JSH_I2C1|JSH_I2C_SDA);
  }
}

/** Addresses are 7 bit - that is, between 0 and 0x7F. sendStop is whether to send a stop bit or not */
void jshI2CWrite(IOEventFlags device, unsigned char address, int nBytes, const unsigned char *data, bool sendStop) {
  const  nrf_drv_twi_t *twi = jshGetTWI(device);
  if (!twi) return;
  uint32_t err_code = nrf_drv_twi_tx(twi, address, data, nBytes, !sendStop);
  if (err_code != NRF_SUCCESS)
    jsExceptionHere(JSET_INTERNALERROR, "I2C Write Error %d\n", err_code);
}

void jshI2CRead(IOEventFlags device, unsigned char address, int nBytes, unsigned char *data, bool sendStop) {
  const nrf_drv_twi_t *twi = jshGetTWI(device);
  if (!twi) return;
  uint32_t err_code = nrf_drv_twi_rx(twi, address, data, nBytes);
  if (err_code != NRF_SUCCESS)
    jsExceptionHere(JSET_INTERNALERROR, "I2C Read Error %d\n", err_code);
}

/// Enable watchdog with a timeout in seconds
void jshEnableWatchDog(JsVarFloat timeout) {
  NRF_WDT->CONFIG = (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) | ( WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos);
  NRF_WDT->CRV = (int)(timeout*32768);
  NRF_WDT->RREN |= WDT_RREN_RR0_Msk;  //Enable reload register 0
  NRF_WDT->TASKS_START = 1;
}

void jshKickWatchDog() {
  NRF_WDT->RR[0] = 0x6E524635;
}

void jshDelayMicroseconds(int microsec) {
  if (microsec <= 0) {
    return;
  }
  nrf_delay_us((uint32_t)microsec);
}

bool utilTimerActive = false;

/// Reschedule the timer (it should already be running) to interrupt after 'period'
void jshUtilTimerReschedule(JsSysTime period) {
  if (period < JSSYSTIME_MAX / NRF_TIMER_FREQ) {
    period = period * NRF_TIMER_FREQ / (long long)SYSCLK_FREQ;
    if (period < 1) period=1;
    if (period > NRF_TIMER_MAX) period=NRF_TIMER_MAX;
  } else {
    // it's too big to do maths on... let's just use the maximum period
    period = NRF_TIMER_MAX;
  }
  //jsiConsolePrintf("Sleep for %d %d -> %d\n", (uint32_t)(t>>32), (uint32_t)(t), (uint32_t)(period));
  if (utilTimerActive) nrf_timer_task_trigger(NRF_TIMER1, NRF_TIMER_TASK_STOP);
  nrf_timer_task_trigger(NRF_TIMER1, NRF_TIMER_TASK_CLEAR);
  nrf_timer_cc_write(NRF_TIMER1, NRF_TIMER_CC_CHANNEL0, (uint32_t)period);
  if (utilTimerActive) nrf_timer_task_trigger(NRF_TIMER1, NRF_TIMER_TASK_START);
}

/// Start the timer and get it to interrupt after 'period'
void jshUtilTimerStart(JsSysTime period) {
  jshUtilTimerReschedule(period);
  if (!utilTimerActive) {
    utilTimerActive = true;
    nrf_timer_task_trigger(NRF_TIMER1, NRF_TIMER_TASK_START);
  }
}

/// Stop the timer
void jshUtilTimerDisable() {
  utilTimerActive = false;
  nrf_timer_task_trigger(NRF_TIMER1, NRF_TIMER_TASK_STOP);
}


