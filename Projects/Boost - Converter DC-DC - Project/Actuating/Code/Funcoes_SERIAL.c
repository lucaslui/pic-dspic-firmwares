// ===============================================================================================================================================================

#include <PIC16F628A.h>
#include "Funcoes_SERIAL.h"

// ===============================================================================================================================================================

unsigned char RecebeSerial(void)
{
    unsigned char valor_recebido = 0;
    RCSTAbits.CREN = 1;         // Liga recebimento serial.
    if(PIR1bits.RCIF == 1)      // Registrador aponta que recebeu algum dado serial? se sim faça.
    {
       valor_recebido = RCREG;	// retorna o valor
       if(RCSTAbits.OERR == 1) RCSTAbits.CREN = 0; // Desliga recebimento serial.
       return valor_recebido;
    }
    if(RCSTAbits.OERR == 1) RCSTAbits.CREN = 0; // Desliga recebimento serial.
    return 0;
}

// ===============================================================================================================================================================

void InicializaSerial(void)
{
    TRISBbits.TRISB1 = 1;  // pino do serial configuado como entrada RX.
    TXSTA = 0b00000100;	   // Modo assíncrono, transmissor desligado, comunicacao em alta velocidade.
    RCSTA = 0b10000000;	   // Porta serial ligada, habilitada receptação dados, configurado 8 bits de dados para receber.
    SPBRG = 25;	           // 25 em decimal valor para velocidade baudrate em 9.6k.
}

// ===============================================================================================================================================================
