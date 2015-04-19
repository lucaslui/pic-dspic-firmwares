/* ==================================================================================================================

	Arquivo: Funcoes_ADC.c
	Descrição: Biblioteca para ADC de 10 bits.
	Portável: Não portável, necessita ajustes dependendo do microcontrolador utilizado.
	Microcontrolador Utilizado: PIC18F2550.

 ==================================================================================================================*/

#include "Definindo_SFRs.h"
#include "Funcoes_ADC.h"

// ==================================================================================================================

#define BitSetar(arg,nBit)    ((arg) |=  (1<<nBit)) // Macros para mascarar os bytes, são feitas operações bit a bit
#define BitLimpar(arg,nBit)   ((arg) &= ~(1<<nBit)) // para setar, limpar, inverter, e testar bits especificos de um byte.
#define BitInverter(arg,nBit) ((arg) ^=  (1<<nBit))
#define BitTestar(arg,nBit)   ((arg) &   (1<<nBit))

// ==================================================================================================================

#define GODONE 1 // Defines para o bom entendimento do código.
#define ADON 0

// ==================================================================================================================


void Inicializando_ADC(void)
{
  ADCON0 = 0b00000000; // Selecionado CANAL 0, ou seja habilita conversão A/D neste primeiro momento no pino AN0.
  ADCON1 = 0b00001101; // Tensões de referência = VDD e GND, AN0 e AN1 configurados como pino analógico.
  ADCON2 = 0b10101110; // Justificado a direita, tempo de aquisição de dados 12 TAD, velocidade conversão FOSC/64.
  BitSetar(ADCON0, ADON);   /*  ADON: bit liga/desliga ADC
                                1 = A/D - Habilita a possibilidade de ligar o ADC.
                                0 = A/D - Desliga o ADC, não a mais comsumo de corrente.*/
}

unsigned int Leitura_ADC(void)
{
  unsigned int valor_adc_temporario = 0;
  BitSetar(ADCON0, GODONE); // GO/DONE: Emite o "GO" da operação de conversão A/D.

  while (BitTestar(ADCON0, GODONE)); // Esperando conversão A/D acabar ou seja status bit igual a 0.
  valor_adc_temporario =  ADRESH ;   // Conversão A/D de 10 bits, sendo assim eles são armazenados em dois registradores, 
  valor_adc_temporario <<=  8 ;      // ADRESH(high) e ADRESHL (low). Em seguida, é atribuido o resultado da parte alta na variável local 
  valor_adc_temporario += ADRESL;    // criada (int - 16 bits), em seguida deslocada 8 vezes a esquerda, colocando-a no devido 
                                     // lugar, em seguida só é atribuida a parte baixa na mesma váriavel.
  return (valor_adc_temporario);     // Retorna valor inteiro (resultado da conversão).
}
