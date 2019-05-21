/*
 * include.h
 *
 *  Created on: May 16, 2019
 *      Author: DINHTOAN
 */

#ifndef INCLUDE_H_
#define INCLUDE_H_

#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define SLAVE_ADDRESS   0x68
#define SEC             0x00
#define MIN             0x01
#define HRS             0x02
#define DAY             0x03
#define DATE            0x04
#define MONTH           0x05
#define YEAR            0x06
#define CNTRL           0x07

#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_i2c.h"
#include "inc/hw_timer.h"
#include "inc/hw_uart.h"

#include "driverlib/systick.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/ssi.h"
#include "utils/uartstdio.h"
#include "uartstdio.h"
#include "driverlib/can.h"
#include "driverlib/eeprom.h"
#include "driverlib/i2c.h"
#include "driverlib/lcd.h"
#include "driverlib/mpu.h"
#include "driverlib/pwm.h"
#include "driverlib/timer.h"
#include "driverlib/adc.h"
#include "driverlib/interrupt.h"
#include "driverlib/qei.h"
#include "driverlib/fpu.h"
#include "driverlib/rom.h"

#include "LCD/lcd_i2c.h"
#include "LIB/Mfrc522.h"
#include "UART/UART.h"
#include "RTC/RTC.h"
#include "TIMER/timer.h"

#endif /* INCLUDE_H_ */
