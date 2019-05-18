/*
 * lcd_i2c.c
 *
 *  Created on: May 16, 2019
 *      Author: DINHTOAN
 */

#include "lcd_i2c.h"
volatile uint8_t addr =0x27;

extern void initI2C(void)
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

  // Set up the pins as I2C
  GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
  GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

  GPIOPinConfigure(GPIO_PB2_I2C0SCL);
  GPIOPinConfigure(GPIO_PB3_I2C0SDA);

  // master enable
  I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), true);//false for 100kHz mode
  I2CMasterEnable(I2C0_BASE);
}

extern void I2C_byte(uint8_t slave_address, uint8_t data) {

  //volatile int i = 0;

  // start I2C transmit
  I2CMasterSlaveAddrSet(I2C0_BASE, slave_address, false);
  I2CMasterDataPut(I2C0_BASE, data);
  while(I2CMasterBusy(I2C0_BASE)) {

  } // Check, the bus isn't busy (low?)

  // Single Byte
  I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
  while(I2CMasterBusy(I2C0_BASE)) {

  }    //wait

}

extern void I2C_write(uint8_t slave_address, uint8_t *data, uint8_t length)
{
  volatile int i = 0;

  if (length == 1) {
    I2C_byte(slave_address, *(data));
    return;
  }

  // start I2C transmit
  I2CMasterSlaveAddrSet(I2C0_BASE, slave_address, false);
  I2CMasterDataPut(I2C0_BASE, *(data));
  while(I2CMasterBusy(I2C0_BASE)) {

  } // Check, the bus isn't busy (low?)
  // Multi Bytes
  I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
  while(I2CMasterBusy(I2C0_BASE)) {
  }

  for(i=1; i < length; i++) {
    I2CMasterDataPut(I2C0_BASE, *(data+i));
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
    while(I2CMasterBusy(I2C0_BASE)) {}
  }
  I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
  while(I2CMasterBusy(I2C0_BASE)) {
  } // delay until complete
}

extern void Delay_lcd(int nCount)
{
  /* Decrement nCount value*/
  while (nCount != 0)
  {
    nCount--;
  }
}

extern void Lcd_write(uint8_t RS,uint8_t RW,uint8_t data,uint8_t led)  {
    uint8_t a = 0,b = 0;
      a = data & 0xF0; // 4 bit high
      b = (data <<4) & 0xF0; //// 4 bit low

      if(RS==1){
          a += 0x01;
          b += 0x01;
      }
      if(RW==1){
          a += 0x02;   // bit 1
          b += 0x02;
      }
       if(led==1){
          a += 0x08;
          b += 0x08;
       }

      a += 0x04;
      I2C_byte(addr,a); // EN = 1
      Delay_lcd(100);
      a -= 0x04 ; // bit 2 , EN = 0
      I2C_byte(addr,a); // EN = 0
      Delay_lcd(100);

      b += 0x04;
            I2C_byte(addr,b); // EN = 1
            Delay_lcd(100);
            b -= 0x04 ; // bit 2 , EN = 0
            I2C_byte(addr,b); // EN = 0
            Delay_lcd(100);

}

extern void Lcd_Goto(unsigned char y, unsigned char x)
{
        uint8_t ad;
        ad = 64*(y-1)+(x-1)+0x80;
        Lcd_write(0,0,ad,1); // ghi lenh
        Delay_lcd(10000);
}

extern void lcd_init(void) {
    Delay_lcd(100);
    Lcd_write(0,0,0x28,1);
    Delay_lcd(100);
    Lcd_write(0,0,0x02,1);
    Delay_lcd(100);// giao thuc 4 bit, hien thi 2 hang, ki tu 5x8
    Lcd_write(0,0,0x0C,1);
    Delay_lcd(100);// cho phep hien thi man hinh
    Lcd_write(0,0,0x0F,1);
    Delay_lcd(100);
    Lcd_write(0,0,0x06,1);
    Delay_lcd(100);// tang ID, khong dich hinh
    Lcd_write(0,0,0x01,1);
    Delay_lcd(100);
}

extern void clear(void) {
    Delay_lcd(100);
    Lcd_write(0,0,0x01,1);

}

extern void Lcd_write_char(uint8_t data,uint8_t led)
{
    Delay_lcd(100);
    Lcd_write(1,0,data,led);
}

extern void LCD_Puts(char *s)
{
        while (*s){

            Lcd_write_char(*s,1);
            Delay_lcd(100);
                s++;
        }
}
