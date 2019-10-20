#include "Funcoes_Basicas.h"
#include "Funcoes_I2C.h"
#include <pic18f4520.h>

#define _XTAL_FREQ 8000000

#define DEVICE_ADRESS_WRITE 0b11010000 // MCP3421 device code is 1101, Address Bits A2- A0 = 000 are  
#define DEVICE_ADRESS_READ  0b11010001 // programmed at factory unless customer requests different codes.

void write_msp(unsigned char data)
 {
    unsigned char recebeu_ack = 0;
    
    while(1)
    {
        i2c_start();
        i2c_write(DEVICE_ADRESS_WRITE);
        recebeu_ack = i2c_write(data);
        i2c_stop();
        delay_ms(10);
        if(recebeu_ack == 1) break;
    }
}

unsigned int read_msp()
{
    unsigned char valor_ad[2], recebeu_ack = 0;
    unsigned int valor_ad_uma_variavel = 0;
    
        i2c_start();
        if (i2c_write(DEVICE_ADRESS_READ) == 0) 
        {
            i2c_stop();
            return 0; 
        }
        valor_ad[1] = i2c_read(1);
        valor_ad[0] = i2c_read(1);
        i2c_stop();
    
    
        valor_ad_uma_variavel = valor_ad[1] << 8;
        valor_ad_uma_variavel += valor_ad[0];
    
    return valor_ad_uma_variavel;
}

/*
void write_ext_eeprom(unsigned int address, unsigned char data)
 {
    unsigned char a0 = ((address & 0x8000) >> 14);  
    unsigned char msb = (address >> 8);
    unsigned char lsb = (address & 0x00FF);

    i2c_start();
    i2c_write(0xa0 | a0);
    i2c_write(msb);
    i2c_write(lsb);
    i2c_write(data);
    i2c_stop();
    delay_ms(11);
}

/******************************************************************************************/

/*unsigned char read_ext_eeprom(unsigned int address)
{
   unsigned char a0 = ((address & 0x8000) >> 14);  
   unsigned char data;
   unsigned char msb = (address >> 8);
   unsigned char lsb = (address & 0x00FF);

   i2c_start();
   i2c_write(0xa0 | a0);
   i2c_write(msb);
   i2c_write(lsb);
   i2c_repStart();
   i2c_write(0xa1 | a0);
   data=i2c_read(0);
   i2c_stop();
   return(data);
}*/

void Inicializando_I2C()
{
    //------------------------ COMUNICAÇÃO I2C ---------------------------------------------------------------------
    
    // SSPSTAT = 0b10000000;   // Slew rate desabilitado, ou seja, Standard Speed mode (100 kHz and 1 MHz).
    // SSPCON1 = 0b00101000;    // Habilita o MSSP (pinos SDA/SCL = serial pinos), I2C Master mode, clock = FOSC/(4 * (SSPADD + 1)).
    
    // MASTER MODE:
    //   SSPCON2bits.RCEN = 1;   // Habilita modo recepção (Master Receive Mode).
    //   SSPCON2bits.PEN = 1;    // Operação STOP feita automaticamente atráves desse bit = 1.
    //   SSPCON2bits.SEN = 1;    // Operação de START feita automaticamente através desse bit = 1.
    //   SSPCON2bits.RSEN = 1;   // Habilita repetidas operações de START quando = 1.
    
    // MASTER RECEIVE MODE:    
    //   SSPCON2bits.ACKEN = 1;  // Sequencia de ACK feita automaticamente pelo hardware com esse bit = 1.
    //   SSPCON2bits.ACKDT = 0;  // Valor que deve ser enviado pela sequencia acima (ACK = 0 e not ACK = 1).
    
    // MASTER TRANSMIT MODE:
    // SSPCON2bits.ACKSTAT = 0; // Status Bit, quando: 0 = ACK recebido, 1 = ACK não-recebido.
    
    //---------------------------------------------------------------------------------------------------------------------
    
    TRISCbits.RC3 = 1;   // set SCL and SDA pins as inputs
    TRISCbits.RC4 = 1;

    SSPCON1 = 0x38;      // Habilita I2C, modo Mestre (Master) com clock = fosc / 4 * (SSPADD + 1).
    SSPCON2 = 0x00;
    SSPADD = ((_XTAL_FREQ/100000)/4) - 1;  // 100kHz bus with 20Mhz xtal
    SSPSTATbits.CKE = 0;  // Disable SMBus specific inputs
    SSPSTATbits.SMP = 1;  // Slew rate control disabled for Standard Speed mode (100 kHz and 1 MHz)

    PIR1bits.PSPIF=0;      // clear SSPIF interrupt flag
    PIR2bits.BCLIF=0;      // clear bus collision flag
}

/******************************************************************************************/

void i2c_waitForIdle()
{
    while((SSPCON2 & 0x1F) | SSPSTATbits.RW ){}; // wait for idle and not writing
}

/******************************************************************************************/

void i2c_start()
{
 i2c_waitForIdle();
 SSPCON2bits.SEN = 1; // 1 = Initiates Start condition on SDA and SCL pins. Automatically cleared by hardware.
}

/******************************************************************************************/

void i2c_repStart()
{
 i2c_waitForIdle();
 SSPCON2bits.RSEN = 1; // 1 = Initiates Repeated Start condition on SDA and SCL pins. Automatically cleared by hardware.
}

/******************************************************************************************/

void i2c_stop()
{
 i2c_waitForIdle();
 SSPCON2bits.PEN = 1; // 1 = Initiates Stop condition on SDA and SCL pins. Automatically cleared by hardware.
}

/******************************************************************************************/

int i2c_read(unsigned char ack)
{
    unsigned char data_read;

    i2c_waitForIdle();
    SSPCON2bits.RCEN = 1; // 1 = Enables Receive mode for I2C
                          // 0 = Receive Idle
    i2c_waitForIdle();
    data_read = SSPBUF;
    i2c_waitForIdle();
    if (ack) SSPCON2bits.ACKDT = 0; // ACKDT bit
                                    // 1 = Not Acknowledge
                                    // 0 = Acknowledge
    else SSPCON2bits.ACKDT = 1;

    SSPCON2bits.ACKEN = 1; // 1 = Initiates Acknowledge sequence on SDA and SCL pins and transmit 
                           //     ACKDT data bit. Automatically cleared by hardware.
                           // 0 = Acknowledge sequence Idle
    return( data_read );
}

/******************************************************************************************/

unsigned char i2c_write( unsigned char i2cWriteData )
{
    i2c_waitForIdle();
    SSPBUF = i2cWriteData;
    return ( ! SSPCON2bits.ACKSTAT  ); // Function returns '1' if transmission is acknowledged
}                                   // ACKSTAT bit:
                                    // 1 = Acknowledge was not received from slave
                                    // 0 = Acknowledge was received from slave
