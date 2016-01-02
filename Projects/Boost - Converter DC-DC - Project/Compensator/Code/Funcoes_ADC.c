// ===============================================================================================================================================================

#include <xc.h>
#include "Funcoes_ADC.h"

// ===============================================================================================================================================================

void Inicializando_ADC(void)
{
  ADCON0 = 0b10000000; // Justificativa a direita, Tensao referencia Vdd, canal 0 (AN0), ADON E GO/DONE desligados.
  ADCON1 = 0b00100000; // A/D Conversion Clock = FOSC/36.(010)

  ADCON0bits.ADON = 1;     //  ADON: bit liga/desliga ADC
                           //   1 = A/D - Habilita a possibilidade de ligar o ADC.
                           //   0 = A/D - Desliga o ADC, não a mais comsumo de corrente.
}

// ===============================================================================================================================================================

unsigned int Leitura_ADC(void)
{
  unsigned int valor_adc_temporario = 0;
  ADCON0bits.GO_nDONE = 1;     // GO/DONE: dando start de conversão.

  while (ADCON0bits.GO_nDONE); // Esperando conversão A/D acabar ou seja status do bit GO_nDone igual a 0.
  valor_adc_temporario =  ADRESH ;
  valor_adc_temporario <<=  8 ;
  valor_adc_temporario += ADRESL;
  return (valor_adc_temporario); //retorna valor inteiro.
}

// ===============================================================================================================================================================