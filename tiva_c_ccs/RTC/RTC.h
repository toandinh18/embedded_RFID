/*
 * RTC.h
 *
 *  Created on: May 17, 2019
 *      Author: DINHTOAN
 */

#ifndef RTC_RTC_H_
#define RTC_RTC_H_

#include "../include.h"
extern void RTC_InitI2C(void);
extern void RTC_I2C_byte(uint8_t slave_address, uint8_t data);
extern void RTC_I2C_write(uint8_t slave_address, uint8_t *data, uint8_t length);
extern uint32_t RTC_I2C_Receive(uint8_t slave_address, uint8_t reg);
extern uint8_t dec2bcd(uint8_t val);
extern uint8_t bcd2dec(uint8_t val);
extern void RTC_Init(void);
extern void SetTimeDate(uint8_t _sec,uint8_t _min,uint8_t _hour,uint8_t _day,uint8_t _date,uint8_t _month,uint8_t _year);
extern uint8_t GetClock(uint8_t reg);

#endif /* RTC_RTC_H_ */
