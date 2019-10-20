//==============================================================================
/*
 *  Código para o receptor do deficiente.
 *  Data ínicio: 10/09/2015.
 */
//==============================================================================

#include <pic18f4550.h>
#include "ConfigBits.h"
#include "CodigoBasicos.h"
#include "CodigoSerial.h"

#define Linha_1  PORTDbits.RD0
#define Linha_2  PORTDbits.RD1
#define Linha_3  PORTDbits.RD2
#define Linha_4  PORTDbits.RD3

#define confirma 13
#define limpa 14
#define ida 16
#define volta 17

#define centena 2
#define dezena 1
#define unidade 0

//==============================================================================

unsigned int linha = 0;
unsigned char sentido = ida;

//==============================================================================

unsigned char LerTecladoMatricial()
{
    Linha_1 = 0; Linha_2 = 1; Linha_3 = 1; Linha_4 = 1;
    delay_ms(1);
    if((DebBotao(&PORTD, 7, 5, 0)) == 1) return 1;
    else if((DebBotao(&PORTD, 6, 5, 0)) == 1) return 2;
         else if((DebBotao(&PORTD, 5, 5, 0)) == 1) return 3;
              else if((DebBotao(&PORTD, 4, 5, 0)) == 1) return confirma;

    Linha_1 = 1; Linha_2 = 0; Linha_3 = 1; Linha_4 = 1;
    delay_ms(1);
    if((DebBotao(&PORTD, 7, 5, 0)) == 1) return 4;
    else if((DebBotao(&PORTD, 6, 5, 0)) == 1) return 5;
         else if((DebBotao(&PORTD, 5, 5, 0)) == 1) return 6;
              else if((DebBotao(&PORTD, 4, 5, 0)) == 1) return limpa;

    Linha_1 = 1; Linha_2 = 1; Linha_3 = 0; Linha_4 = 1;
    delay_ms(1);
    if((DebBotao(&PORTD, 7, 5, 0)) == 1) return 7;
    else if((DebBotao(&PORTD, 6, 5, 0)) == 1) return 8;
         else if((DebBotao(&PORTD, 5, 5, 0)) == 1) return 9;
              else if((DebBotao(&PORTD, 4, 5, 0)) == 1) return 255;

    Linha_1 = 1; Linha_2 = 1; Linha_3 = 1; Linha_4 = 0;
    delay_ms(1);
    if((DebBotao(&PORTD, 7, 5, 0)) == 1) return volta;
    else if((DebBotao(&PORTD, 6, 5, 0)) == 1) return 0;
         else if((DebBotao(&PORTD, 5, 5, 0)) == 1) return ida;
              else if((DebBotao(&PORTD, 4, 5, 0)) == 1) return 255;

    return 255;
}

void Toque_BIP(char numero_toques)
{
    for(char i = numero_toques; i > 0; i--)
    {
        for(unsigned char t = 0; t < 100; t++)
        {
            PORTBbits.RB6 = 1;
            for(char j = 0; j < 70; j++);
            PORTBbits.RB6 = 0;
            for(char x = 0; x < 70; x++);
        }
        if(i != 0) delay_ms(200);
    }
}

void Config_Linha()
{
    unsigned char valor_teclado = 255, confirmacao = 0, digito_selecionado = centena;
    unsigned char unidade_var = 0, dezena_var = 0, centena_var = 0;
    unsigned int linha_anterior = 0;

    linha = 0;

    while(1)
    {
        valor_teclado = 255;
        valor_teclado = LerTecladoMatricial();

        if(valor_teclado != 255 && valor_teclado != confirma) Toque_BIP(1);

        switch (valor_teclado)
        {
            case 0:
                switch(digito_selecionado)
                {
                    case centena:
                        centena_var = 0; digito_selecionado = dezena;
                        break;
                    case dezena:
                        dezena_var = 0; digito_selecionado = unidade;
                        break;
                    case unidade:
                        unidade_var = 0; digito_selecionado = unidade;
                        break;
                }
                break;

            case 1:
                switch(digito_selecionado)
                {
                    case centena:
                        centena_var = 1; digito_selecionado = dezena;
                        break;
                    case dezena:
                        dezena_var = 1; digito_selecionado = unidade;
                        break;
                    case unidade:
                        unidade_var = 1; digito_selecionado = unidade;
                }
                break;

            case 2:
                switch(digito_selecionado)
                {
                    case centena:
                        centena_var = 2; digito_selecionado = dezena;
                        break;
                    case dezena:
                        dezena_var = 2;  digito_selecionado = unidade;
                        break;
                    case unidade:
                        unidade_var = 2; digito_selecionado = unidade;
                }
                break;

            case 3:
                switch(digito_selecionado)
                {
                    case centena:
                        centena_var = 3; digito_selecionado = dezena;
                        break;
                    case dezena:
                        dezena_var = 3; digito_selecionado = unidade;
                        break;
                    case unidade:
                        unidade_var = 3; digito_selecionado = unidade;
                }
                break;

            case 4:
                switch(digito_selecionado)
                {
                    case centena:
                        centena_var = 4; digito_selecionado = dezena;
                        break;
                    case dezena:
                        dezena_var = 4; digito_selecionado = unidade;
                        break;
                    case unidade:
                        unidade_var = 4; digito_selecionado = unidade;
                }
                break;

            case 5:
                switch(digito_selecionado)
                {
                    case centena:
                        centena_var = 5; digito_selecionado = dezena;
                        break;
                    case dezena:
                        dezena_var = 5; digito_selecionado = unidade;
                        break;
                    case unidade:
                        unidade_var = 5; digito_selecionado = unidade;
                }
                break;

            case 6:
                switch(digito_selecionado)
                {
                    case centena:
                        centena_var = 6; digito_selecionado = dezena;
                        break;
                    case dezena:
                        dezena_var = 6; digito_selecionado = unidade;
                        break;
                    case unidade:
                        unidade_var = 6; digito_selecionado = unidade;
                }
                break;

            case 7:
                switch(digito_selecionado)
                {
                    case centena:
                        centena_var = 7;
                        digito_selecionado = dezena;
                        break;
                    case dezena:
                        dezena_var = 7;
                        digito_selecionado = unidade;
                        break;
                    case unidade:
                        unidade_var = 7;
                        digito_selecionado = unidade;
                }
                break;

            case 8:
                switch(digito_selecionado)
                {
                    case centena:
                        centena_var = 8; digito_selecionado = dezena;
                        break;
                    case dezena:
                        dezena_var = 8; digito_selecionado = unidade;
                        break;
                    case unidade:
                        unidade_var = 8; digito_selecionado = unidade;
                }
                break;

            case 9:
                switch(digito_selecionado)
                {
                    case centena:
                        centena_var = 9; digito_selecionado = dezena;
                        break;
                    case dezena:
                        dezena_var = 9; digito_selecionado = unidade;
                        break;
                    case unidade:
                        unidade_var = 9; digito_selecionado = unidade;
                }
                break;

            case confirma:  // Botão "COMFIRMA" foi pressionado.
                if(confirmacao == 0)
                {
                    linha_anterior = ((centena_var*100) + (dezena_var*10) + (unidade_var));
                    confirmacao = 1;
                    Toque_BIP(1);
                    centena_var = 0; dezena_var = 0; unidade_var = 0;
                    digito_selecionado = centena;
                }
                else if(confirmacao == 1)
                     {
                          linha = ((centena_var*100) + (dezena_var*10) + (unidade_var));
                          if (linha == linha_anterior)
                          {
                            Toque_BIP(2);
                            return;
                          }
                          else
                          {
                              confirmacao = 0;
                              Toque_BIP(3);
                              centena_var = 0; dezena_var = 0; unidade_var = 0;
                              digito_selecionado = centena;
                          }
                     }
                break;

            case limpa: // Botão "Limpa" foi pressionado.
                confirmacao = 0;
                centena_var = 0; dezena_var = 0; unidade_var = 0;
                digito_selecionado = centena;
                break;
        }
    }
}

void Config_Sentido()
{
    unsigned char valor_teclado = 255;
    unsigned char temp_sentido = ida;

    while(1)
    {
        valor_teclado = 255;
        valor_teclado = LerTecladoMatricial();

        switch(valor_teclado)
        {
            case ida:
                temp_sentido = ida;
                Toque_BIP(1);
                break;

            case volta:
                temp_sentido = volta;
                Toque_BIP(1);
                break;

            case confirma:
                sentido = temp_sentido;
                Toque_BIP(2);
                return;
        }
    }
}

void Inicializando_SFR()
{
    OSCCON = 0b01110011; // Habilitando oscilador interno como 8Mhz.
    CMCON = 0b00000111; // Desabilitando comparadores.
    TRISA = 0xFF; // PORTA totalmente como entrada (alta impedância).
    TRISB = 0xFF; // PORTB totalmente configurado como entrada (alta impedância).
    TRISC = 0XFF; // PORTC totalmente como entrada (alta impedância).
    TRISD = 0b11110000; // Habilitando os botoes do teclado matricial.

    INTCON = 0x00; // Desabilitando todo tipo de interrupção.
    PORTEbits.RDPU = 0;
    TRISBbits.TRISB6 = 0; // Saída para campainha.
}

void main()
{
    Inicializando_SFR();
    Toque_BIP(2);
    Config_Linha();
    Config_Sentido();
    Inicializa_Serial();
    unsigned char recebe_serial = 0;

    while(1)
    {
        recebe_serial = Recebe_Serial();
        if(linha == recebe_serial) Toque_BIP(5);
        Envia_Serial(linha);
    }
}

//==============================================================================