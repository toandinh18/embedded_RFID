/*
 * Mfrc522.c
 *
 *  Created on: May 14, 2019
 *      Author: DINHTOAN
 */


#include <stdbool.h>
#include <stdint.h>
#include "Mfrc522.h"
#include "../include.h"
#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#define CHIPSELECT_BASE  0x40005000     //GPIO_PORTB_BASE
#define NRSTPD_BASE    0x40025000       //GPIO_PORTF_BASE
#define SSI_BASE       0x4000A000      //SSI2_BASE

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
        UARTprintf("%x ", buffer[i]);
    }
    UARTprintf("  FIM! \r\n"); //End
}


uint8_t SPI_transfer(uint8_t data){
       uint32_t rxtxData;

        rxtxData = data;

        SSIDataPut(SSI_BASE, (uint8_t) rxtxData);

        while(SSIBusy(SSI_BASE));

        SSIDataGet(SSI_BASE, &rxtxData);

        return (uint8_t) rxtxData;
}

extern void WriteReg(unsigned char addr, unsigned char val)
{
    GPIOPinWrite(CHIPSELECT_BASE, chipSelectPin, 0);
    //digitalWrite(chipSelectPin, 0);

    SPI_transfer((addr<<1)&0x7E);
    SPI_transfer(val);

    GPIOPinWrite(CHIPSELECT_BASE, chipSelectPin, chipSelectPin);
    //digitalWrite(chipSelectPin, 1);
}

extern unsigned char ReadReg(unsigned char addr)
{
    unsigned char val;
    GPIOPinWrite(CHIPSELECT_BASE, chipSelectPin, 0);
    //digitalWrite(chipSelectPin, 0);

    SPI_transfer(((addr<<1)&0x7E) | 0x80);
    val = SPI_transfer(0x00);

    GPIOPinWrite(CHIPSELECT_BASE, chipSelectPin, chipSelectPin);
    //digitalWrite(chipSelectPin, 1);

    return val;
}

extern void SetBitMask(unsigned char reg, unsigned char mask)
{
    unsigned char tmp;
    tmp = ReadReg(reg);
    WriteReg(reg, tmp | mask);  // set bit mask
}

extern void ClearBitMask(unsigned char reg, unsigned char mask)
{
    unsigned char tmp;
    tmp = ReadReg(reg);
    WriteReg(reg, tmp & (~mask));  // clear bit mask
}

extern void AntennaOn(void)
{
    unsigned char temp;

    temp = ReadReg(TxControlReg);
    if (!(temp & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}

extern void AntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);
}

extern void Reset(void)
{
    WriteReg(CommandReg, PCD_RESETPHASE);
}

extern void Init(void)
{
    GPIOPinWrite(NRSTPD_BASE, NRSTPD, NRSTPD);
    //digitalWrite(NRSTPD,1);

    Reset();

    //Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
    WriteReg(TModeReg, 0x8D);       //Tauto=1; f(Timer) = 6.78MHz/TPreScaler
    WriteReg(TPrescalerReg, 0x3E);  //TModeReg[3..0] + TPrescalerReg
    WriteReg(TReloadRegL, 30);
    WriteReg(TReloadRegH, 0);

    WriteReg(TxAutoReg, 0x40);      //100%ASK
    WriteReg(ModeReg, 0x3D);

    //ClearBitMask(Status2Reg, 0x08);       //MFCrypto1On=0
    //WriteReg(RxSelReg, 0x86);     //RxWait = RxSelReg[5..0]
    //WriteReg(RFCfgReg, 0x7F);         //RxGain = 48dB

    AntennaOn();
}

extern unsigned char Request(unsigned char reqMode, unsigned char *TagType)
{
    unsigned char status;
    unsigned int backBits;

    WriteReg(BitFramingReg, 0x07);      //TxLastBists = BitFramingReg[2..0] ???

    TagType[0] = reqMode;
    status = ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

    if ((status != MI_OK) || (backBits != 0x10))
    {
        status = MI_ERR;
    }

    return status;
}

extern unsigned char ToCard(unsigned char command, unsigned char *sendData, unsigned char sendLen, unsigned char *backData, unsigned int *backLen)
{
    unsigned char status = MI_ERR;
    unsigned char irqEn = 0x00;
    unsigned char waitIRq = 0x00;
    unsigned char lastBits;
    unsigned char n;
    unsigned int i;

    switch (command)
    {
        case PCD_AUTHENT:
        {
            irqEn = 0x12;
            waitIRq = 0x10;
            break;
        }
        case PCD_TRANSCEIVE:
        {
            irqEn = 0x77;
            waitIRq = 0x30;
            break;
        }
        default:
            break;
    }

    WriteReg(CommIEnReg, irqEn|0x80);
    ClearBitMask(CommIrqReg, 0x80);
    SetBitMask(FIFOLevelReg, 0x80);

    WriteReg(CommandReg, PCD_IDLE);

    for (i=0; i<sendLen; i++)
    {
        WriteReg(FIFODataReg, sendData[i]);
    }

    WriteReg(CommandReg, command);
    if (command == PCD_TRANSCEIVE)
    {
        SetBitMask(BitFramingReg, 0x80);        //StartSend=1,transmission of data starts
    }

    i = 10000;
    do
    {
        n = ReadReg(CommIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitIRq));

    ClearBitMask(BitFramingReg, 0x80);          //StartSend=0

    if (i != 0)
    {
        if(!(ReadReg(ErrorReg) & 0x1B)) //BufferOvfl Collerr CRCErr ProtecolErr
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {
                status = MI_NOTAGERR;           //??
            }

            if (command == PCD_TRANSCEIVE)
            {
                n = ReadReg(FIFOLevelReg);
                lastBits = ReadReg(ControlReg) & 0x07;
                if (lastBits)
                {
                    *backLen = (n-1)*8 + lastBits;
                }
                else
                {
                    *backLen = n*8;
                }
                if (n == 0)
                {
                    n = 1;
                }
                if (n > MAX_LEN)
                {
                    n = MAX_LEN;
                }

                for (i=0; i<n; i++)
                {
                    backData[i] = ReadReg(FIFODataReg);
                }
            }
        }
        else
        {
            status = MI_ERR;
        }
    }

//SetBitMask(ControlReg,0x80);           //timer stops
//WriteReg(CommandReg, PCD_IDLE);

    return status;
}

extern unsigned char Anticoll(unsigned char *serNum)
{
    unsigned char status;
    unsigned char i;
    unsigned char serNumCheck=0;
    unsigned int unLen;


//ClearBitMask(Status2Reg, 0x08);       //TempSensclear
//ClearBitMask(CollReg,0x80);           //ValuesAfterColl
    WriteReg(BitFramingReg, 0x00);      //TxLastBists = BitFramingReg[2..0]
    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

    if (status == MI_OK)
    {
        for (i=0; i<4; i++)
        {
            serNumCheck ^= serNum[i];
        }
        if (serNumCheck != serNum[i])
        {
            status = MI_ERR;
        }
    }

    //SetBitMask(CollReg, 0x80);        //ValuesAfterColl=1

    return status;
}

extern void CalulateCRC(unsigned char *pIndata, unsigned char len, unsigned char *pOutData)
{
    unsigned char i, n;

    ClearBitMask(DivIrqReg, 0x04);          //CRCIrq = 0
    SetBitMask(FIFOLevelReg, 0x80);
    //WriteReg(CommandReg, PCD_IDLE);

    for (i=0; i<len; i++)
    {
        WriteReg(FIFODataReg, *(pIndata+i));
    }
    WriteReg(CommandReg, PCD_CALCCRC);

    i = 0xFF;
    do
    {
        n = ReadReg(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));            //CRCIrq = 1

    pOutData[0] = ReadReg(CRCResultRegL);
    pOutData[1] = ReadReg(CRCResultRegM);
}

extern unsigned char SelectTag(unsigned char *serNum)
{
    unsigned char i;
    unsigned char status;
    unsigned char size;
    unsigned int recvBits;
    unsigned char buffer[9];

    //ClearBitMask(Status2Reg, 0x08);           //MFCrypto1On=0

    buffer[0] = PICC_SElECTTAG;
    buffer[1] = 0x70;
    for (i=0; i<5; i++)
    {
        buffer[i+2] = *(serNum+i);
    }
    CalulateCRC(buffer, 7, &buffer[7]);     //??
    status = ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);

    if ((status == MI_OK) && (recvBits == 0x18))
    {
        size = buffer[0];
    }
    else
    {
        size = 0;
    }

    return size;
}

extern unsigned char Auth(unsigned char authMode, unsigned char BlockAddr, unsigned char *Sectorkey, unsigned char *serNum)
{
    unsigned char status;
    unsigned int recvBits;
    unsigned char i;
    unsigned char buff[12];

    buff[0] = authMode;
    buff[1] = BlockAddr;
    for (i=0; i<6; i++)
    {
        buff[i+2] = *(Sectorkey+i);
    }
    for (i=0; i<4; i++)
    {
        buff[i+8] = *(serNum+i);
    }
    status = ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);

    if ((status != MI_OK) || (!(ReadReg(Status2Reg) & 0x08)))
    {
        status = MI_ERR;
    }

    return status;
}

extern unsigned char ReadBlock(unsigned char blockAddr, unsigned char *recvData)
{
    unsigned char status;
    unsigned int unLen;

    recvData[0] = PICC_READ;
    recvData[1] = blockAddr;
    CalulateCRC(recvData,2, &recvData[2]);
    status = ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

    if ((status != MI_OK) || (unLen != 0x90))
    {
        status = MI_ERR;
    }

    return status;
}

extern unsigned char WriteBlock(unsigned char blockAddr, unsigned char *writeData)
{
    unsigned char status;
    unsigned int recvBits;
    unsigned char i;
    unsigned char buff[18];

    buff[0] = PICC_WRITE;
    buff[1] = blockAddr;
    CalulateCRC(buff, 2, &buff[2]);
    status = ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

    if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    if (status == MI_OK)
    {
        for (i=0; i<16; i++)
        {
            buff[i] = *(writeData+i);
        }
        CalulateCRC(buff, 16, &buff[16]);
        status = ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);

        if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
        {
            status = MI_ERR;
        }
    }

    return status;
}

extern void Halt(void)
{
    unsigned char status;
    unsigned int unLen;
    unsigned char buff[4];

    buff[0] = PICC_HALT;
    buff[1] = 0;
    CalulateCRC(buff, 2, &buff[2]);

    status = ToCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);
}





