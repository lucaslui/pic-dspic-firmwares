//==============================================================================

#include <pic18f4550.h>
#include "CodigoBasicos.h"
#include "CodigoSerial.h"

//==============================================================================

void Envia_Serial(unsigned char dados)
{
    while(PIR1bits.TXIF == 0);	// aguarda o registro ficar disponível
    TXREG = dados; 		// coloca o valor para ser enviado
}

//==============================================================================

unsigned char Recebe_Serial(void)
{
    unsigned char valor_recebido = 0;
    RCSTAbits.CREN = 1; // Liga recebimento serial.
    while(PIR1bits.RCIF == 0);	// Só sai se receber um valor na porta serial.
    valor_recebido = RCREG;	// retorna o valor
    if(RCSTAbits.OERR == 1) RCSTAbits.CREN = 0; // Desliga recebimento serial.
    return valor_recebido;
}

//==============================================================================

void Inicializa_Serial(void)
{
    TXSTA = 0b00100000;	   // 8 bits de transmissao,  transmissor ligado, modo 
                           // assíncrono, baixa velocidade.

    RCSTA = 0b10000000;	   // Funcionamento serial ligado, 8 bits de recepcao,
                           // recepção desligada, desabilitada endereçamento.
    
    BAUDCON = 0b00000000;  // Sem auto-badrate, recepção de data não é invertido
                           // tramissão também, 8 bits de baudrate gerador SPBRG
                           // sem wake-up interrupção na recepção.

    BAUDCTL = 0b00000000;  // modulo gerador baudrate com 16 bits. sem auto baudrate e sem estouro com auto baudrate.
    SPBRG = 12;	    // 12 em decimal valor para velocidade baudrate em 9.6k.

    TRISCbits.TRISC6 = 0;   // pino digital configuado como saida TX.
    TRISCbits.TRISC7 = 1;   // pino digital configuado como entrada RX.
}