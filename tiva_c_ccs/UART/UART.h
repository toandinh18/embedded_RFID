/*
 * UART.h
 *
 *  Created on: May 16, 2019
 *      Author: DINHTOAN
 */

#ifndef UART_UART_H_
#define UART_UART_H_

extern void InitConsole(void);
extern void UART_Config_UART1(void);
extern void InitSSI();
extern void initLeds();
extern void dumpHex(unsigned char* buffer, int len);



#endif /* UART_UART_H_ */
