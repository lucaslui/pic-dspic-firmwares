/*
 * Arquivo: Funcoes_Basicas.h
 * Autor: Lucas
 */

#ifndef FUNCOES_BASICAS_H
#define	FUNCOES_BASICAS_H

void delay_ms(int ms);
char DebBotao(volatile near unsigned char *PORT, char nBit, int deboucingTime, char changeState);

#endif FUNCOES_BASICAS_H


