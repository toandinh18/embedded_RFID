
#include "include.h"

#define redLED   0x00000002
#define blueLED  0x00000004
#define greenLED 0x00000008

#define CARD_LENGTH     5
#define chipSelectPin   0x20
#define NRSTPD          0x01

char title[30]="Nhom 1";

volatile uint8_t sec,min,hour,day,date,month, year;

uint8_t Version;
uint8_t AntennaGain;
uint8_t status;
uint32_t readTeste;
unsigned char str[MAX_LEN];
unsigned char cardID[CARD_LENGTH];

    int main(void) {
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ); //80MHz
    /// khoi tao UART0 va UART1
    TimerBegin();
    InitConsole();
    UART_Config_UART1();
    initLeds(); // config led
    InitSSI();  // config i2c cho RF522
    initI2C();  // config i2c cho LCD

    RTC_InitI2C();
    //RTC_Init();
    //SetTimeDate(00,39,17,6,24,5,19); //30 giay 32 phut 8h thu4 ngay 5 thang 11

    lcd_init();
    clear();
    HWREG(I2C0_BASE + I2C_O_FIFOCTL) = 80008000;
    Lcd_Goto(1,5);
    LCD_Puts(title);
    GPIOPinWrite(GPIO_PORTB_BASE, chipSelectPin, 0);
    GPIOPinWrite(GPIO_PORTF_BASE, NRSTPD, NRSTPD);

    Init();
    Version = ReadReg(VersionReg);
    AntennaGain = ReadReg(PICC_REQIDL) & (0x07<<4);

    UARTprintf("Version: '0x%x' \n", Version);
    UARTprintf("Antenna Gain: '0x%x' \n\n", AntennaGain);

    while(1){
        uint8_t i;
        status = Request(PICC_REQIDL, str);
        if(status == MI_OK){
            UARTprintf("Card Detected! \n"); //Card Detected
            GPIOPinWrite(GPIO_PORTF_BASE, blueLED, blueLED);
            status = Anticoll(str);
            memcpy(cardID, str, 5);
            if(status == MI_OK){
                clear();
                Lcd_Goto(2,5);
                LCD_Puts("DONE");
                Wait(400);
                clear();
                UARTprintf("ID: \n");
                for (i=0;i<5;i++) {
                   UARTCharPut(UART1_BASE,cardID[i]);
                   Wait(1);
                }
                UARTCharPut(UART1_BASE,GetClock(HRS));
                Wait(1);
                UARTCharPut(UART1_BASE,GetClock(MIN));
                Wait(1);
                UARTCharPut(UART1_BASE,GetClock(SEC));
                Wait(1);
                UARTCharPut(UART1_BASE,GetClock(DAY));
                Wait(1);
                UARTCharPut(UART1_BASE,GetClock(DATE));
                Wait(1);
                UARTCharPut(UART1_BASE,GetClock(MONTH));
                Wait(1);
                UARTprintf("%02d : %02d : %02d \n",GetClock(HRS),GetClock(MIN),GetClock(SEC));
                GPIOPinWrite(GPIO_PORTF_BASE, blueLED, 0);
                Wait(2); //Delay
           }
       }
    }
}

