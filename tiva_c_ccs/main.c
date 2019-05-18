
#include "include.h"

#define redLED   0x00000002
#define blueLED  0x00000004
#define greenLED 0x00000008

#define CARD_LENGTH     10
#define chipSelectPin   0x20
#define NRSTPD          0x01

volatile uint8_t sec,min,hour,day,date,month, year;
uint8_t Version;
uint8_t AntennaGain;
uint8_t status;
uint32_t readTeste;
unsigned char str[MAX_LEN];
unsigned char cardID[CARD_LENGTH];

int main(void) {
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ); //40MHz
    InitConsole();
    UART_Config_UART1();
    initLeds();
    InitSSI();
    initI2C();

    RTC_InitI2C();
    RTC_Init();
    SetTimeDate(10,10,10,4,10,10,19); //30 giay 32 phut 8h thu4 ngay 5 thang 11

//    lcd_init();
//    clear();
//    Lcd_Goto(1,5);
//    LCD_Puts("cc m");
//    GPIOPinWrite(GPIO_PORTB_BASE, chipSelectPin, 0);
//    GPIOPinWrite(GPIO_PORTF_BASE, NRSTPD, NRSTPD);
//
//    Init();
//
//    Version = ReadReg(VersionReg);
//    AntennaGain = ReadReg(PICC_REQIDL) & (0x07<<4);
//
//    UARTprintf("Version: '0x%x' \n", Version);
//    UARTprintf("Antenna Gain: '0x%x' \n\n", AntennaGain);

    while(1){
        sec = GetClock(SEC);
        min = GetClock(MIN);
        hour= GetClock(HRS);
        day = GetClock(DAY);
        date = GetClock(DATE);
        month = GetClock(MONTH);
        year = GetClock(YEAR);
        SysCtlDelay(SysCtlClockGet()/10*2);
        UARTprintf("%02d : %02d : %02d \n%02d / %02d / %02d / %02d \n",hour,min,sec,day,date,month,year);


//    uint8_t i=0;
//    status = Request(PICC_REQIDL, str);
//    if(status == MI_OK){
//        UARTprintf("Card Detected! \n"); //Card Detected
//        GPIOPinWrite(GPIO_PORTF_BASE, blueLED, blueLED);
//    }
//
//    status = Anticoll(str);
//    memcpy(cardID, str, 10);
//
//    if(status == MI_OK){
//        clear();
//        Lcd_Goto(2,5);
//        LCD_Puts("DONE");
//        UARTprintf("ID: \n");
//        while(cardID[i]!=0) {
//           UARTCharPut(UART1_BASE,cardID[i]);
//           i++;
//        }
//        dumpHex((unsigned char*)cardID, 5);
//        GPIOPinWrite(GPIO_PORTF_BASE, blueLED, 0);
//        SysCtlDelay(SysCtlClockGet()/2); //Delay
//     }
  }
}

