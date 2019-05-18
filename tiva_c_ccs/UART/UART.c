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
//        UARTIntRegister(UART1_BASE,&UART_IRS1);
//        IntEnable(INT_UART1);
//        UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);
//        IntMasterEnable();
}

extern void InitSSI() {
    uint32_t junkAuxVar;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); //SDA
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //reset

    GPIOPinConfigure(GPIO_PB4_SSI2CLK);
    GPIOPinConfigure(GPIO_PB5_SSI2FSS);
    GPIOPinConfigure(GPIO_PB6_SSI2RX);
    GPIOPinConfigure(GPIO_PB7_SSI2TX);

    GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_6 |
                   GPIO_PIN_7);

    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_5); //chipSelectPin
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0); //NRSTPD

    //
    SSIConfigSetExpClk(SSI2_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 4000000, 8);
    //
    // Enable the SSI0 module.
    //
    SSIEnable(SSI2_BASE);

    while(SSIDataGetNonBlocking(SSI2_BASE, &junkAuxVar)){}

    UARTprintf("SSI Enabled! SPI Mode!  \nData: 8bits.\n\n");

}

extern void initLeds() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
}


extern void dumpHex(unsigned char* buffer, int len) {
    int i;

    UARTprintf(" ");
    for(i=0; i < len; i++) {
        UARTprintf("%x", buffer[i]);
    }
    UARTprintf("  FIM! \r\n"); //End
}
