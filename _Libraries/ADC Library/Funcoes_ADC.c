#include "Definindo_SFRs.h"
#include "Funcoes_ADC.h"

// ===============================================================================================================================================================

#define BitSetar(arg,nBit)    ((arg) |=  (1<<nBit)) // Definições para operações bit a bit
#define BitLimpar(arg,nBit)   ((arg) &= ~(1<<nBit)) // para setar,limpar,inverter,e testar bits especificos de um byte.
#define BitInverter(arg,nBit) ((arg) ^=  (1<<nBit))
#define BitTestar(arg,nBit)   ((arg) &   (1<<nBit))

#define GODONE 1
#define ADON 0

// ===============================================================================================================================================================


void Inicializando_ADC(void)
{
  ADCON0 = 0b00000000; // Selecionado CANAL 0, ou seja habilitando conversao A/D em AN0.
  ADCON1 = 0b00001101; // Vdd = tensão referencia, AN0 e AN1 configurados como pino analógico.
  ADCON2 = 0b10101110;// Justificado a direita, tempo de aquisição de dados 12 TAD, velocidade conversão FOSC/64.
  BitSetar(ADCON0, ADON);   /*  ADON: bit liga/desliga ADC
                           1 = A/D - Habilita a possibilidade de ligar o ADC.
                           0 = A/D - Desliga o ADC, não a mais comsumo de corrente.*/
}

unsigned int Leitura_ADC(void)
{
  unsigned int valor_adc_temporario = 0;
  BitSetar(ADCON0, GODONE); // GO/DONE: A/D Status bit.

  while (BitTestar(ADCON0, GODONE)); // Esperando conversão A/D acabar ou seja status bit igual a 0.
  valor_adc_temporario =  ADRESH ;
  valor_adc_temporario <<=  8 ;
  valor_adc_temporario += ADRESL;
  return (valor_adc_temporario); //retorna valor inteiro.
}
