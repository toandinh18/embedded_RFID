/*
 * lcd_i2c.h
 *
 *  Created on: May 16, 2019
 *      Author: DINHTOAN
 */

#ifndef LCD_LCD_I2C_H_
#define LCD_LCD_I2C_H_

#include "../include.h"
extern void initI2C(void);
extern void I2C_byte(uint8_t slave_address, uint8_t data);
extern void I2C_write(uint8_t slave_address, uint8_t *data, uint8_t length);
extern void Delay_lcd(int nCount);
extern void Lcd_write(uint8_t RS,uint8_t RW,uint8_t data,uint8_t led);
extern void Lcd_Goto(unsigned char y, unsigned char x);
extern void lcd_init(void);
extern void clear(void);
extern void Lcd_write_char(uint8_t data,uint8_t led);
extern void LCD_Puts(char *s);

#endif /* LCD_LCD_I2C_H_ */
