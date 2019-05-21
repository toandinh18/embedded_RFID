/*
 * RTC.c
 *
 *  Created on: May 17, 2019
 *      Author: DINHTOAN
 */

#include "RTC.h"


extern void RTC_InitI2C(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA6_I2C1SCL);
    GPIOPinConfigure(GPIO_PA7_I2C1SDA);
    GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);
    GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);
    I2CMasterInitExpClk(I2C1_BASE, SysCtlClockGet(), true);
    HWREG(I2C1_BASE + I2C_O_FIFOCTL) = 80008000;
}

extern void RTC_I2C_byte(uint8_t slave_address, uint8_t data) {
    I2CMasterSlaveAddrSet(I2C1_BASE, slave_address, false);
    I2CMasterDataPut(I2C1_BASE, data);
    while(I2CMasterBusy(I2C1_BASE)) {

      } // Check, the bus isn't busy (low?)
      // Single Byte
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    while(I2CMasterBusy(I2C1_BASE)) {
      }    //wait
}

extern void RTC_I2C_write(uint8_t slave_address, uint8_t *data, uint8_t length)
{
  volatile int i = 0;

  if (length == 1) {
    RTC_I2C_byte(slave_address, *(data));
    return;
  }

  // start I2C transmit
  I2CMasterSlaveAddrSet(I2C1_BASE, slave_address, false);
  I2CMasterDataPut(I2C1_BASE, *(data));
  while(I2CMasterBusy(I2C1_BASE)) {

  } // Check, the bus isn't busy (low?)
  // Multi Bytes
  I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START);
  while(I2CMasterBusy(I2C1_BASE)) {
  }

  for(i=1; i < length; i++) {
    I2CMasterDataPut(I2C1_BASE, *(data+i));
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
    while(I2CMasterBusy(I2C1_BASE)) {}
  }
  I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
  while(I2CMasterBusy(I2C1_BASE)) {
  } // delay until complete
}


extern uint32_t RTC_I2C_Receive(uint8_t slave_address, uint8_t reg) {
    //specify that we are writing (a register address) to the
       //slave device
       I2CMasterSlaveAddrSet(I2C1_BASE, slave_address, false);
       //specify register to be read
       I2CMasterDataPut(I2C1_BASE, reg);
       //send control byte and register address byte to slave device
       I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START);
       //wait for MCU to finish transaction
       while(I2CMasterBusy(I2C1_BASE)){};
       //specify that we are going to read from slave device
       I2CMasterSlaveAddrSet(I2C1_BASE, slave_address, true);
       //send control byte and read from the register we
       //specified
       I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
       //wait for MCU to finish transaction
       while(I2CMasterBusy(I2C1_BASE)) {};
       //return data pulled from the specified register
       return I2CMasterDataGet(I2C1_BASE);
}

extern uint8_t dec2bcd(uint8_t val) {
    return (((val / 10) << 4) | (val % 10));
}

extern uint8_t bcd2dec(uint8_t val) {
  return (((val & 0xF0) >> 4) * 10) + (val & 0x0F);
}

extern void RTC_Init(void) {
    uint8_t second =0;
    second=bcd2dec(RTC_I2C_Receive(SLAVE_ADDRESS,SEC));
    second &=0x7F;
    RTC_I2C_byte(SLAVE_ADDRESS,SEC);
    RTC_I2C_byte(SLAVE_ADDRESS,dec2bcd(second));
    RTC_I2C_byte(SLAVE_ADDRESS,CNTRL);
    RTC_I2C_byte(SLAVE_ADDRESS,0x80);
}

extern void SetTimeDate(uint8_t _sec, uint8_t _min, uint8_t _hour, uint8_t _day, uint8_t _date, uint8_t _month, uint8_t _year) {
    uint8_t buff[8];
    _sec &=0x7F;
    _hour &=0x3F;
    buff[0]=SEC;
    buff[1]=dec2bcd(_sec);
    buff[2]=dec2bcd(_min);
    buff[3]=dec2bcd(_hour);
    buff[4]=dec2bcd(_day);
    buff[5]=dec2bcd(_date);
    buff[6]=dec2bcd(_month);
    buff[7]=dec2bcd(_year);
    RTC_I2C_write(SLAVE_ADDRESS,buff,8);
 }

extern uint8_t GetClock(uint8_t reg) {
      uint8_t clockData = RTC_I2C_Receive(SLAVE_ADDRESS,reg);
      return bcd2dec(clockData);
 }
