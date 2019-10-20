/* ==================================================================================================================
	Arquivo: DELAY.c
	Descrição: Biblioteca básica com funções básicas como delay, e tratamento de botões.
	Portável: Função delay ainda não portável, função do tratamento de botões está portável.
	Microcontrolador Utilizado: PIC18F2550.
 ==================================================================================================================*/

#include "DELAY.h"

#define BitSetar(arg,nBit)    ((arg) |=  (1<<nBit)) // Macros para mascarar os bytes, são feitas operações bit a bit
#define BitLimpar(arg,nBit)   ((arg) &= ~(1<<nBit)) // para setar, limpar, inverter, e testar bits especificos de um byte.
#define BitInverter(arg,nBit) ((arg) ^=  (1<<nBit))
#define BitTestar(arg,nBit)   ((arg) &   (1<<nBit))

// ==================================================================================================================

void delay_ms(int ms)
{
  /* 14 instruções para uma incrementação
   * 1/4MHz = 250ns, 4 ciclos para efetuar uma instrução no PIC
   * Assim 250ns x 4 = 1us por instrução,
   * ocorrem 14 por incrementação sendo assim 14us por incremento
   * para gerar 0,5s de delay precisaram 35714 incrementos.
   * logo ms 1 ms equivale a 71,4285 incrementos. Sendo assim:  */
  unsigned int numeroIncrementos , i = 0;
  numeroIncrementos = (ms * 71);
  for (i = 0; i < numeroIncrementos; i++);
}
