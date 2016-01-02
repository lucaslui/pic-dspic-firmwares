/*==================================================================================================================
 *
 *             Algoritmo/Programa para um projeto de uma balança digital.
 *                             MCU utilizado: PIC18F4431
 *
 *  Autor: Lucas Lui Motta.
 *
 *  Projeto: Balança Eletrônica - a balança contém um sensor de peso de 0-40kg, com uma interface de display de
 *           7-segmentos de 5 dígitos.
 *
 *  Softwares usados: XC8 - Compilador.
 *                    MPLABX - IDE.
 *		      PICKIT2 - Gravador.
 *		      Proteus Isis - Simulator.
 *
 * ==================================================================================================================
 *
 *  Sobre Licença: GNU GPL 2
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 * ==================================================================================================================
 *
 *  Arquivo: Main_Code.c
 *
 *  Arquivos do projeto: Main_Code.c
 *                       Fusiveis_Externos.h
 *                       Macros_Mascara.h
 *                       Funcoes_ADC.h
 *                       Funcoes_ADC.c
 */
// ==================================================================================================================

#include <p18f4431.h>
#include "Fusiveis_Externos.h"
#include "Macros_Mascara.h"
#include "Funcoes_ADC.h"

// ==================================================================================================================

#define zero    0b01000000; // G desligado,  numero 0 no display.
#define um      0b01111001; // B e C ligado, numero 1 no display.
#define dois    0b00100100; // F e C desligado, numero 2 no display.
#define tres    0b00110000; // F e E desligado, numero 3 no display.
#define quatro  0b00011001; // A, D, E desligado, numero 4 no display.
#define cinco   0b00010010; // B e E desligado, numero 5 no display.
#define seis    0b00000010; // B desligaado, numero 6 no display.
#define sete    0b01111000; // A, B, C ligado, numero 7 no display.
#define oito    0b00000000; // Tudo ligado,  numero 8 no display.
#define nove    0b00010000; // E desligado,  numero 9 no display

// ==================================================================================================================

unsigned int unidade = 0, dezena = 0, centena = 0,  milhar = 0, dezena_milhar = 0;
unsigned char display_alvo = 0, i = 0, n = 0;
float valor_temporario = 0;
unsigned int valor_em_kg = 0, somatorias_valor_adc = 0, resultado_final = 0;

// ==================================================================================================================

void Transformando_em_kg(unsigned int valor)
{   /*    1.40 kg ferramentas - 1,881 volts   *
    *     2,31 kg - 1,985 volts               *
    *     1,716 volts -  sem nada             */
    if (valor < 356) // Valor em decimal da conversão AD que representa a Tara ou valor de inicio de pesagem.
    {
      valor_em_kg = 0;
    }
    else
    {
      valor_temporario = 0;
      valor_em_kg = 0;
      valor_temporario = (float) (0.0414 * valor);
      valor_temporario = (float) (valor_temporario - 14.484);
      valor_em_kg = (int) (valor_temporario * 1000);
    }
    unidade = 0;
    dezena = 0;
    centena = 0;
    milhar = 0;
    dezena_milhar = 0;
    unidade = (int) (valor_em_kg % 10);            // Arranca o valor da unidade do resultado da conversão A/D.
    dezena = (int) ((valor_em_kg % 100) / 10);     // Arranca o valor da dezena  do resultado da conversão A/D.
    centena = (int) ((valor_em_kg % 1000) / 100);  // Arranca o valor da centena do resultado da conversão A/D.
    milhar = (int) ((valor_em_kg % 10000) / 1000); // Arranca o valor da milhar  do resultado da conversão A/D.
    dezena_milhar = (int) (valor_em_kg / 10000);
}

unsigned char numeroDisplay(unsigned char numero) 
{
  switch(numero)
  {
   case 0:
     return zero;
   case 1:
     return um;
   case 2:
     return dois;
   case 3:
     return tres;
   case 4:
     return quatro;
   case 5:
     return cinco;
   case 6:
     return seis;
   case 7:
     return sete;
   case 8:
     return oito;
   case 9:
     return nove;
  }
  return (0);
}

void MediaAritmetica(unsigned int valor_ad)
{
  somatorias_valor_adc += valor_ad;
  n++;
}

void ComutacaoDisplay(void) // Procedimento de multiplexação dos display.
{                           // Projeto se inicializou com 5 display de 7 segmentos de 1 unidade, posteriormente foi retirada um digito menor.
  switch(display_alvo)
  {
   case 0:
     PORTCbits.RC1 = 0;
     PORTB = 0;
     PORTB = numeroDisplay(unidade);
     PORTCbits.RC0 = 1;
     display_alvo = 1;
     break;
   case 1:
     PORTCbits.RC0 = 0;
     PORTB = 0;
     PORTB = numeroDisplay(dezena);
     PORTAbits.RA3 = 1;
     display_alvo = 2;
     break;
   case 2:
     PORTAbits.RA3 = 0;
     PORTB = 0;
     PORTB = numeroDisplay(centena);
     PORTEbits.RE0 = 1;
     display_alvo = 3;
     break;
   case 3:
     PORTEbits.RE0 = 0;
     PORTB = 0;
     PORTB = numeroDisplay(milhar);
     PORTAbits.RA1 = 1;
     display_alvo = 4;
     break;
   case 4:
     PORTAbits.RA1 = 0;
     PORTB = 0;
     PORTB = numeroDisplay(dezena_milhar);
     PORTCbits.RC1 = 1;
     display_alvo = 0;
     break;
  }
}

void interrupt TMR0_TMR1(void)
{
    if (INTCONbits.TMR0IF == 1) // Interrupção causada por estouro do timer0, no qual
    {                           // está ligado a multiplexação dos displays.
      INTCONbits.TMR0IF = 0;
      TMR0L = 0x50;       // Reinicializando valores.
      TMR0H = 0xF5;
      ComutacaoDisplay(); // Troca de digito do display a cada 2ms
      return;
    }

    if (PIR1bits.TMR1IF == 1)  // Flag de estouro da interrupção do timer1.
    {
      PIR1bits.TMR1IF = 0;
      TMR1L = 0x83;            // Reinicializando valores.
      TMR1H = 0xCD;           
      T1CONbits.TMR1ON = 0;    // TMR1 desligado.
      T0CONbits.TMR0ON = 0;    // TMR0 desligado.
      MediaAritmetica(LeituraADC());
      if(n == 50)
      {
        resultado_final = (int)(somatorias_valor_adc / 50);
        Transformando_em_kg(resultado_final);
        n = 0;
        somatorias_valor_adc = 0;
      }
      T0CONbits.TMR0ON = 1;   // TMR0 ligado.
      T1CONbits.TMR1ON = 1;   // TMR1 ligado.
      return;
    }
}

void InicializandoSFRs(void)
{
  OSCCON = 0b01110110;
  TRISA =  0b11110101; // RA1 e RA3 configurado como saída, demais pinos configurados como entrada.
  TRISB =  0x00;       // Pinos do PORTB configurados como saida.
  TRISC =  0b11111100; // RC0, RC1 como saida.
  TRISEbits.RE0 = 0;   // Pino RE0 configurado como saída.
  CCP1CON = 0x00;      // Modo CCP desligado.
  CCP2CON = 0x00;      // Modo CCP desligado.
}

void InicializandoInterrupcoes(void)
{
  RCONbits.IPEN = 0;      // Desabilita prioridades de interrupções.
  INTCONbits.GIE = 1;     // Ligando chave geral de interrupções.
  INTCONbits.PEIE = 1;    // Ligando chave geral das interrupções de periférico.
  T0CON = 0x00;           // TMR0 modo 16 bit, Incremento por Instrução clock interno e prescaler de 1:2.
  T1CON = 0b10110000;     // TMR1 modo 16 bit, Incremento por Instrução clock interno e prescaler de 1:8.
  INTCONbits.TMR0IE = 1;  // Ligando Interrupção TMR0 para ser usado no display.
  PIE1bits.TMR1IE = 1;    // Ligando Interrupção TMR1 para ser usado na conversao A/D.

  TMR0L = 0x50; // Inicializando TMR0 no modo 16 bits que equivale a 65535, logo a cada instrução esse valor se incrementara
  TMR0H = 0xF5; // sendo assim, precisaremos de 25000 incrementos com prescaler de 1:2 para gerar um estou do TMR0 de 5ms,
                // para isso se carrega o valor inicial deles com 65525-62800 = 2725, em hexadecimal de F550 .

  TMR1L = 0x50; // Inicializando TMR1 no modo 16 bits que equivale a 65535, logo a cada instrução esse valor se incrementara
  TMR1H = 0xC3; // sendo assim, precisaremos de 37500 incrementos com prescaler de 1:8 para gerar um estouro do TMR1 de 112ms.
                // para isso se carrega o valor inicial deles com 65535-50000 = 28035, em hexadeciaml  C350 .

  T0CONbits.TMR0ON = 1;   // TMR0 ligado.
  T1CONbits.TMR1ON = 1;   // TMR1 ligado.
}

void main(void)
{
  InicializandoSFRs();         // Procedimento de inicialização de alguns SFR's.
  InicializandoADC();          // Procedimento de inicialização da conversão A/D.
  InicializandoInterrupcoes(); // Procedimento de inicialização dos registradores da interrupção.
  while(1);
}

// ==================================================================================================================
