#include <xc.h>
#include "serial.h"

void EnviaSerial(unsigned char dados)
{
    while(PIR1bits.TXIF == 0);	// aguarda o registro ficar disponível
    TXREG = dados; 		// coloca o valor para ser enviado
}

void InicializaSerial(void)
{
    TXSTA = 0b00100000;	   // modo assíncrono, 8 bits de transmissao, transmissor ligado, alta velocidade.
    RCSTA = 0b10000000;	   // porta serial ligada, desabilitada receptação dados.
    BAUDCTL = 0b00000000;  // modulo gerador baudrate com 16 bits. sem auto baudrate e sem estouro com auto baudrate.
    SPBRG = 51;	    // 51 em decimal valor para velocidade baudrate em 1200.
    TRISCbits.TRISC4 = 0;   // pino do serial configuado como saida TX.
}
