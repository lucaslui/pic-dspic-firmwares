/* =============================================================================
	Arquivo: Codigo_Basicos.h
	Descrição: Arquivo HEADER para administração das funções que serão passadas.
 ===============================================================================
 
#ifndef Codigo_Basicos /* PROTÓTIPOS PARA A PASSAGEM DE FUNÇÕES */
#define	Codigo_Basicos /* PROTÓTIPOS PARA A PASSAGEM DE FUNÇÕES */

void delay_ms(int ms);
char DebBotao(volatile unsigned char *PORT, char nBit, int deboucingTime, char changeState);

#endif /* PROTÓTIPOS PARA A PASSAGEM DE FUNÇÕES */

// =============================================================================
