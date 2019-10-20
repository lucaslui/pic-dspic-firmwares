#include <xc.h>
#include "serial.h"

unsigned char RecebeSerial(void)
{
    unsigned char valor_recebido = 0;
    RCSTAbits.CREN = 1; // Liga recebimento serial.
    while(PIR1bits.RCIF == 0);	// Só sai se receber um valor na porta serial.
    valor_recebido = RCREG;	// retorna o valor
    if(RCSTAbits.OERR == 1) RCSTAbits.CREN = 0; // Desliga recebimento serial.
    return valor_recebido;
}

void InicializaSerial(void)
{
    TRISBbits.TRISB1 = 1;   // pino do serial configuado como entrada RX.
    TRISBbits.TRISB2 = 1;   // pino do serial configuado como entrada RX.
    TXSTA = 0b00000000;	   // modo assíncrono, 8 bits de transmissao, transmissor desligado, baixa velocidade.
    RCSTA = 0b10000000;	   // porta serial ligada, habilitada receptação dados.
    SPBRG = 51;	    // 51 em decimal valor para velocidade baudrate em 1.2k.
}
