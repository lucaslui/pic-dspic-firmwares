/* ==================================================================================================================
*
* Arquivo: Funcoes_ADC.c
* Descrição: Biblioteca para ADC de 10 bits.
* Portável: Não portável, necessita ajustes dependendo do microcontrolador utilizado.
* Microcontrolador Utilizado: PIC18F4431.
*
* ==================================================================================================================*/

#include <p18f4431.h>
#include "Funcoes_ADC.h"
#include "Macros_Mascara.h"

// ==================================================================================================================

void InicializandoADC(void)
{
  ANSEL0 = 0x01;       // RA0 configurado como analógico (AN0)
  ANSEL1 = 0x00;       // Demais entradas configuradas como Digital I/O.
  ADCON1 = 0x00;       // Referencia no AVdd e AVss.
  ADCON0 = 0x00;       // Single-Shot mode enabled, Single Channel mode enabled, Multi-Channel mode disabled,Single Channel Mode 1 (SCM1); Group A is taken and converted(1)
  ADCON2 = 0b10110010; // Justificativa a direita, delay de conversão como 10TAD, Frequencia de conversão FOSC/64.
  ADCON3 = 0x00;
  ADCHS  = 0x00;       // Selecionado Grupo A entrada AN0.
  ADCON0bits.ADON = 1;   /*  ADON: bit liga/desliga ADC
                           1 = A/D - Habilita a possibilidade de ligar o ADC.
                           0 = A/D - Desliga o ADC, não a mais comsumo de corrente.*/
}

unsigned int LeituraADC(void)
{
  unsigned int valor_adc_temporario = 0;
  ADCON0bits.GO_NOT_DONE = 1; /* GO/DONE: A/D Status bit
                                 Quando ADON = 1:
                                 1 = A/D - Conversão em progresso (Bit automaticamente limpo após concluir conversão).
                                 0 = A/D - Conversão não mais em progresso.*/
  while(ADCON0bits.GO_NOT_DONE != 0); // Esperando conversão A/D acabar ou seja status bit igual a 0.
  valor_adc_temporario =  ADRESH ;
  valor_adc_temporario = (valor_adc_temporario << 8) ;
  valor_adc_temporario = (valor_adc_temporario + ADRESL);
  return (valor_adc_temporario); //retorna valor inteiro.
}

// ==================================================================================================================
