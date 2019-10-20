/* ==================================================================================================================
	Arquivo: Funcoes_Basicas.h
	Descrição: Arquivo HEADER para administração das funções que serão passadas.
 ==================================================================================================================*/
 
#ifndef FUNCOES_BASICAS_Lui /* PROTÓTIPOS PARA A PASSAGEM DE FUNÇÕES */
#define	FUNCOES_BASICAS_Lui /* PROTÓTIPOS PARA A PASSAGEM DE FUNÇÕES */

void delay_ms(int ms);
char DebBotao(volatile near unsigned char *PORT, char nBit, int deboucingTime, char changeState);

#endif /* PROTÓTIPOS PARA A PASSAGEM DE FUNÇÕES */

// ==================================================================================================================

