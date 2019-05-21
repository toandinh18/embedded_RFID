/*
 * UART.c
 *
 *  Created on: May 16, 2019
 *      Author: DINHTOAN
 */

#include "../include.h"
#include "UART.h"

extern void InitConsole(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTStdioConfig(0, 115200, 16000000);

}

extern void UART_Config_UART1(void) {
        SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
        GPIOPinConfigure(GPIO_PB0_U1RX);
        GPIOPinConfigure(GPIO_PB1_U1TX);
        GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
        UARTClockSourceSet(UART1_BASE, UART_CLOCK_SYSTEM);
      //  chân reset high
      //  GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_5,1);
        UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200,UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |UART_CONFIG_PAR_NONE);
        UARTEnable(UART1_BASE);
}



